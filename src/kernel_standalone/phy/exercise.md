# Exercise: Implementing a PHY Driver for the Linux Kernel

## Objective
Develop a basic PHY (Physical Layer) driver for the Linux kernel, demonstrating understanding of kernel module development and network device drivers.

## Background
PHY drivers in the Linux kernel manage the physical layer of network interfaces. They handle tasks such as link detection, speed negotiation, and status reporting.

## Requirements

1. Create a kernel module that implements a PHY driver for a hypothetical Ethernet PHY chip.
2. The driver should include the following components:
   - PHY identification (ID and mask)
   - Configuration initialization
   - Status reading
   - Interrupt handling
   - Soft reset functionality
3. Use the standard Linux kernel PHY driver framework.
4. Implement the following functions:
   - `config_init`: Initialize PHY-specific configurations
   - `read_status`: Read and update PHY status
   - `handle_interrupt`: Handle PHY interrupts
   - `soft_reset`: Perform a soft reset of the PHY
5. Use appropriate kernel APIs and follow Linux kernel coding standards.

## Detailed Steps

1. Set up the basic structure of the kernel module, including necessary headers and module information.
2. Define the PHY ID and mask for your hypothetical PHY chip.
3. Implement the required functions (`config_init`, `read_status`, etc.) with placeholder logic.
4. Create the `struct phy_driver` array with appropriate function pointers and PHY information.
5. Use the `module_phy_driver` macro to register your PHY driver.
6. Implement basic error handling and debugging output using kernel logging functions.

## Bonus Challenges

1. Add support for custom PHY registers and operations.
2. Implement power management functions (`suspend` and `resume`).
3. Add proc file system entries to display PHY status information.

## Evaluation Criteria

- Correct implementation of the PHY driver structure.
- Proper use of kernel APIs and data structures.
- Adherence to Linux kernel coding style and best practices.
- Correct registration and initialization of the PHY driver.
- Proper error handling and resource management.
- Quality of comments and code documentation.

## Submission

Provide the following:
1. The complete source code for your PHY driver (`example_phy_driver.c`).
2. A brief document explaining your implementation choices and any assumptions made.
3. A Makefile for compiling the kernel module.

## Tips

- Refer to existing PHY drivers in the Linux kernel source for examples and best practices.
- Use `ethtool` commands to test your driver's functionality once implemented.
- Remember to handle potential race conditions and use appropriate locking mechanisms.
