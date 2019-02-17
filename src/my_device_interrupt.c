/*
 * Created by IntelliJ IDEA.
 * Project: LED-Gpio-Interrupt-Linux-Device-Driver
 * ===========================================
 * User: ByeongGil Jung
 * Date: 2019-02-12
 * Time: 오후 3:27
 */

/*
< GPIO input / ouput 설정 >
GPIO 입력설정 : gpio_direction_input(gpio_num);
GPIO 입력 : gpio_get_value(gpio_num);
GPIO 출력설정 : gpio_direction_ouput(gpio_num,  init_val); (nit_val 는 초기값)
GPIO 출력 : gpio_set_value(gpio_num, val); (val 는 0, 1)
*/

/*
< 주요 인터럽트 핸들러 플래그 >
IRQ_DISABLED : 인터럽트 핸들러를 실행하는 동안 모든 인터럽트 비활성화
IRQF_SAMPLE_RANDOM : 난수를 적용한 인터럽트로 타이머 인터럽트는 사용 불가
IRQF_TIMER : 시스템 타이머를 적용한 인터럽트
IRQF_SHARED : 인터럽트 공유로 참여하는 모든 핸들러가 이 플래그를 사용해야 함
IRQF_TRIGGER_RISING : 신호가 상승할 때 인터럽트 인지.
IRQF_TRIGGER_FALLING : 신호가 하강할 때 인터럽트 인지.
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

#define LED_ADDRESS 171  // gpio number
#define BUTTON_ADDRESS 174

#define BUFF_SIZE 1024
#define MAJOR_NUMBER 100

// Interrupt handler
// #define IRQF_TRIGGER_NONE       0x00000000
// #define IRQF_TRIGGER_RISING     0x00000001
// #define IRQF_TRIGGER_FALLING    0x00000002
// #define IRQF_TRIGGER_HIGH       0x00000004
// #define IRQF_TRIGGER_LOW        0x00000008
// #define IRQF_TRIGGER_MASK       (IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
//                                  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
// #define IRQF_TRIGGER_PROBE      0x00000010

static const char device_name[] = "my_device_interrupt";
static const char device_irq_name[] = "my_device_irq";
static int *buffer = NULL;
static int gpio_irq_num = 0;

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
    printk(KERN_ALERT "[MyDevice] my_device interrupt function is called\n");

    int button_state, LED_state;

    // Change the LED value for getting the value
    if (gpio_direction_input(LED_ADDRESS) != 0)
        printk(KERN_ALERT "[MyDevice] LED direction error\n");

    button_state = gpio_get_value(BUTTON_ADDRESS);
    LED_state = gpio_get_value(LED_ADDRESS);

    // Change the LED direction for putting value
    if (gpio_direction_output(LED_ADDRESS, 0) != 0)
        printk(KERN_ALERT "[MyDevice] LED direction error\n");

    // If button click, change the LED value
    if (button_state == 0) {
        printk(KERN_ALERT "[MyDevice] button is released\n");
    } else if (button_state == 1) {
        printk(KERN_ALERT "[MyDevice] button is pressed\n");
        switch (LED_state) {
            // Off -> On
            case 0:
                gpio_set_value(LED_ADDRESS, 1);
                printk(KERN_ALERT "[MyDevice] LED off -> LED on\n");
                break;
            // On -> Off
            case 1:
                gpio_set_value(LED_ADDRESS, 0);
                printk(KERN_ALERT "[MyDevice] LED on -> LED off\n");
                break;
            default:
                printk(KERN_ALERT "[MyDevice] Unknown LED state\n");
                break;
        }
    }

    udelay(1000);

    return IRQ_HANDLED;
}

int set_my_device_interrupt_handler(void)
{
    printk(KERN_ALERT "[MyDevice] setting interrupt handler function is called\n");

    int irq_result = 0;

    gpio_irq_num = gpio_to_irq(BUTTON_ADDRESS);

    // Set irq
    irq_result = request_irq(gpio_irq_num, &my_interrupt, IRQF_TRIGGER_RISING, device_irq_name, NULL);
    if (irq_result) {
        printk(KERN_ALERT "[MyDevice] coudn't get irq with gpio_num : %d\n", BUTTON_ADDRESS);
    }

    printk(KERN_ALERT "[MyDevice] setting interrupt handler function is success\n");

    return irq_result;
}

/*************************************************************************/

static int my_open(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "[MyDevice] my_device open function is called\n");

    return 0;
}

static int my_close(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "[MyDevice] my_device close function is called\n");

    return 0;
}

static long my_ioctl(struct inode *inode, unsigned int cmd, unsigned long arg)
{
    printk(KERN_ALERT "[MyDevice] my_device ioctl function is called\n");
    printk(KERN_ALERT "[MyDevice] my_ioctl - command: %x, argument: %x", cmd, (int) arg);

    switch (cmd) {
        case 0:
            // LED off
            if (arg == 0) {
                gpio_set_value(LED_ADDRESS, 0);
                printk(KERN_ALERT "[MyDevice] ioctl - LED off\n");
            // LED on
            } else {
                gpio_set_value(LED_ADDRESS, 1);
                printk(KERN_ALERT "[MyDevice] ioctl - LED on\n");
            }
            break;
        default:
            printk(KERN_ALERT "[MyDevice] it's unknown command\n");
            break;
    }

    return 0;
}

static struct file_operations my_ops = {
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};

int __init my_device_init(void)
{
    printk(KERN_ALERT "[MyDevice] my_device init function is called\n");

    int result;

    // Register my_deivce (character device)
    result = register_chrdev(MAJOR_NUMBER, device_name, &my_ops);
    if (result < 0) {
        printk(KERN_ALERT "[MyDevice] my_device init function is failed\n");
        return result;  // return error
    }

    // Request GPIO_ADDRESS
    // (해당 gpio 를 사용하겠다는 의미. -> 나중에 gpio_free(num) 으로 풀어주어야 함)
    if (gpio_request(LED_ADDRESS, "my_device_led") != 0)
        printk(KERN_ALERT "[MyDevice] gpio_request LED error\n");
    if (gpio_request(BUTTON_ADDRESS, "my_device_button") != 0)
        printk(KERN_ALERT "[MyDevice] gpio_request BUTTON error\n");

    // Set gpio direction
    if (gpio_direction_output(LED_ADDRESS, 0) != 0)
        printk(KERN_ALERT "[MyDevice] LED direction error\n");
    if (gpio_direction_input(BUTTON_ADDRESS) != 0)
        printk(KERN_ALERT "[MyDevice] button direction error\n");

    // Set interrupt handler
    result = set_my_device_interrupt_handler();
    if (result < 0) {
        printk(KERN_ALERT "[MyDevice] setting intterupt handler function is failed\n");
        return result;  // return error
    }

    // Set buffer memory
    buffer = (int*) kmalloc(BUFF_SIZE, GFP_KERNEL);
    if (buffer != NULL)
        memset(buffer, 0, BUFF_SIZE);

    printk(KERN_ALERT "[MyDevice] my_device init successful with result : %d\n", result);

    return 0;
}

void __init my_device_exit(void)
{
    printk(KERN_ALERT "[MyDevice] my_device exit function is called\n");

    gpio_free(LED_ADDRESS);
    gpio_free(BUTTON_ADDRESS);
    free_irq(gpio_irq_num, NULL);

    unregister_chrdev(MAJOR_NUMBER, device_name);
    printk(KERN_ALERT "[MyDevice] my_device cleanup successful\n");

    kfree(buffer);
}

module_init(my_device_init);
module_exit(my_device_exit);
MODULE_LICENSE("GPL");
