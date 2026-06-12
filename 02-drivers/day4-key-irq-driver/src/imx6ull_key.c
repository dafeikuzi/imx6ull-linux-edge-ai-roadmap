#include <linux/atomic.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

#define IMX6ULL_KEY_EVENT_SIZE 32

struct imx6ull_key {
	struct device *dev;
	struct gpio_desc *gpiod;
	int irq;
	atomic_t event_pending;
	char event_text[IMX6ULL_KEY_EVENT_SIZE];
	spinlock_t lock;
	wait_queue_head_t wait;
	struct miscdevice miscdev;
};

static irqreturn_t imx6ull_key_irq(int irq, void *data)
{
	struct imx6ull_key *key = data;
	int value = gpiod_get_value(key->gpiod);
	unsigned long flags;

	spin_lock_irqsave(&key->lock, flags);
	snprintf(key->event_text, sizeof(key->event_text), "%s value=%d\n",
		 value ? "released" : "pressed", value ? 1 : 0);
	atomic_set(&key->event_pending, 1);
	spin_unlock_irqrestore(&key->lock, flags);

	wake_up_interruptible(&key->wait);
	return IRQ_HANDLED;
}

static ssize_t imx6ull_key_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct imx6ull_key *key = container_of(file->private_data,
					       struct imx6ull_key, miscdev);
	char event[IMX6ULL_KEY_EVENT_SIZE];
	unsigned long flags;
	size_t len;
	int ret;

	if (!atomic_read(&key->event_pending)) {
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_interruptible(key->wait,
					       atomic_read(&key->event_pending));
		if (ret)
			return ret;
	}

	spin_lock_irqsave(&key->lock, flags);
	strlcpy(event, key->event_text, sizeof(event));
	atomic_set(&key->event_pending, 0);
	spin_unlock_irqrestore(&key->lock, flags);

	len = strlen(event);
	if (count < len)
		return -EINVAL;

	if (copy_to_user(buf, event, len))
		return -EFAULT;

	return len;
}

static unsigned int imx6ull_key_poll(struct file *file, poll_table *wait)
{
	struct imx6ull_key *key = container_of(file->private_data,
					       struct imx6ull_key, miscdev);
	unsigned int mask = 0;

	poll_wait(file, &key->wait, wait);
	if (atomic_read(&key->event_pending))
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static const struct file_operations imx6ull_key_fops = {
	.owner = THIS_MODULE,
	.read = imx6ull_key_read,
	.poll = imx6ull_key_poll,
	.llseek = no_llseek,
};

static int imx6ull_key_probe(struct platform_device *pdev)
{
	struct imx6ull_key *key;
	int value;
	int ret;

	key = devm_kzalloc(&pdev->dev, sizeof(*key), GFP_KERNEL);
	if (!key)
		return -ENOMEM;

	key->dev = &pdev->dev;
	spin_lock_init(&key->lock);
	init_waitqueue_head(&key->wait);
	atomic_set(&key->event_pending, 0);

	key->gpiod = devm_gpiod_get(&pdev->dev, "key", GPIOD_IN);
	if (IS_ERR(key->gpiod)) {
		ret = PTR_ERR(key->gpiod);
		dev_err(&pdev->dev, "failed to get key gpio: %d\n", ret);
		return ret;
	}

	key->irq = gpiod_to_irq(key->gpiod);
	if (key->irq < 0) {
		dev_err(&pdev->dev, "failed to map gpio to irq: %d\n", key->irq);
		return key->irq;
	}

	value = gpiod_get_value(key->gpiod);
	snprintf(key->event_text, sizeof(key->event_text), "initial value=%d\n",
		 value ? 1 : 0);

	ret = devm_request_threaded_irq(&pdev->dev, key->irq, NULL,
					imx6ull_key_irq,
					IRQF_TRIGGER_RISING |
					IRQF_TRIGGER_FALLING |
					IRQF_ONESHOT,
					"imx6ull-key", key);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq %d: %d\n",
			key->irq, ret);
		return ret;
	}

	key->miscdev.minor = MISC_DYNAMIC_MINOR;
	key->miscdev.name = "imx6ull_key";
	key->miscdev.fops = &imx6ull_key_fops;

	ret = misc_register(&key->miscdev);
	if (ret) {
		dev_err(&pdev->dev, "failed to register misc device: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, key);
	dev_info(&pdev->dev, "imx6ull key irq driver probed, irq=%d value=%d dev=/dev/%s\n",
		 key->irq, value ? 1 : 0, key->miscdev.name);
	return 0;
}

static int imx6ull_key_remove(struct platform_device *pdev)
{
	struct imx6ull_key *key = platform_get_drvdata(pdev);

	misc_deregister(&key->miscdev);
	dev_info(&pdev->dev, "imx6ull key irq driver removed\n");
	return 0;
}

static const struct of_device_id imx6ull_key_of_match[] = {
	{ .compatible = "demo,imx6ull-key" },
	{ }
};
MODULE_DEVICE_TABLE(of, imx6ull_key_of_match);

static struct platform_driver imx6ull_key_driver = {
	.probe = imx6ull_key_probe,
	.remove = imx6ull_key_remove,
	.driver = {
		.name = "imx6ull-key",
		.of_match_table = imx6ull_key_of_match,
	},
};
module_platform_driver(imx6ull_key_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lzw");
MODULE_DESCRIPTION("IMX6ULL demo key irq driver");
