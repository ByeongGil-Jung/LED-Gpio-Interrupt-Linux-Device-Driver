# LED-Gpio-Interrupt-Linux-Device-Driver

## 1. Command

(In /src directory)

### 1) Build the kernel source

```
make
```

### 2) Insert module & Make module

- Make sure that MAJOR_NUMBER (== 100) does not overlap with any other driver.

```
insmod my_device_interrupt.ko
mknod /dev/my_device_interrupt c 100 0
```

(In /run directory)

### 3) Build the application source

- Because it is based on the ARM board, it must be built using the ARM compiler.

```
arm-linux-gnueabihf-gcc run.c
```

## 2. Circuit

- Purple : LED (gpio_171)
- Blue (in) : switch (gpio_174)
- Green : ground
- Black : power

![circuit_1](https://github.com/ByeongGil-Jung/LED-Gpio-Interrupt-Linux-Device-Driver/blob/master/github/img/circuit_1.jpg)  
![circuit_2](https://github.com/ByeongGil-Jung/LED-Gpio-Interrupt-Linux-Device-Driver/blob/master/github/img/circuit_2.jpg)  
![circuit_3](https://github.com/ByeongGil-Jung/LED-Gpio-Interrupt-Linux-Device-Driver/blob/master/github/img/circuit_3.jpg)