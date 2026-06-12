#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>

struct imx6ull_led {
	struct device *dev;
	struct gpio_desc *gpiod;
	struct miscdevice miscdev;
};

static ssize_t imx6ull_led_write(struct file *file, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	struct imx6ull_led *led = container_of(file->private_data,
					       struct imx6ull_led, miscdev);
	char kbuf[16];
	size_t len;
	bool on;

	if (count < 1)
		return -EINVAL;

	len = min(count, sizeof(kbuf) - 1);
	if (copy_from_user(kbuf, buf, len))
		return -EFAULT;
	kbuf[len] = '\0';

	if (sysfs_streq(kbuf, "1") || sysfs_streq(kbuf, "on"))
		on = true;
	else if (sysfs_streq(kbuf, "0") || sysfs_streq(kbuf, "off"))
		on = false;
	else
		return -EINVAL;

	gpiod_set_value(led->gpiod, on);
	dev_info(led->dev, "led %s\n", on ? "on" : "off");

	return count;
}

static ssize_t imx6ull_led_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct imx6ull_led *led = container_of(file->private_data,
					       struct imx6ull_led, miscdev);
	char kbuf[4];
	int value;
	int len;

	if (*ppos)
		return 0;

	value = gpiod_get_value(led->gpiod);
	len = scnprintf(kbuf, sizeof(kbuf), "%d\n", value ? 1 : 0);

	if (count < len)
		return -EINVAL;

	if (copy_to_user(buf, kbuf, len))
		return -EFAULT;

	*ppos += len;
	return len;
}

static const struct file_operations imx6ull_led_fops = {
	.owner = THIS_MODULE,
	.read = imx6ull_led_read,
	.write = imx6ull_led_write,
	.llseek = no_llseek,
};

static int imx6ull_led_probe(struct platform_device *pdev)
{
	struct imx6ull_led *led;
	int ret;

	led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	led->dev = &pdev->dev;
	led->gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
	if (IS_ERR(led->gpiod)) {
		ret = PTR_ERR(led->gpiod);
		dev_err(&pdev->dev, "failed to get led gpio: %d\n", ret);
		return ret;
	}
	led->miscdev.minor = MISC_DYNAMIC_MINOR;
	led->miscdev.name = "imx6ull_led";
	led->miscdev.fops = &imx6ull_led_fops;

	ret = misc_register(&led->miscdev);
	if (ret) {
		dev_err(&pdev->dev, "failed to register misc device: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, led);
	dev_info(&pdev->dev, "imx6ull led driver probed, dev=/dev/%s\n",
		 led->miscdev.name);
	return 0;
}

static int imx6ull_led_remove(struct platform_device *pdev)
{
	struct imx6ull_led *led = platform_get_drvdata(pdev);

	misc_deregister(&led->miscdev);
	dev_info(&pdev->dev, "imx6ull led driver removed\n");
	return 0;
}

static const struct of_device_id imx6ull_led_of_match[] = {
	{ .compatible = "demo,imx6ull-led" },
	{ }
};
MODULE_DEVICE_TABLE(of, imx6ull_led_of_match);

static struct platform_driver imx6ull_led_driver = {
	.probe = imx6ull_led_probe,
	.remove = imx6ull_led_remove,
	.driver = {
		.name = "imx6ull-led",
		.of_match_table = imx6ull_led_of_match,
	},
};
module_platform_driver(imx6ull_led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lzw");
MODULE_DESCRIPTION("IMX6ULL demo LED platform driver");
