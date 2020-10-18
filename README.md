# SwitchTime
Change NetworkSystemClock

Mod of Switch-Time by 3096, for personal use so L/R can change by 7 days and "+" doesn't exit as I intend this as a userprofile redirect on my girlfriend's Switch for her animal crossing needs, along with some things "stripped" so she doesn't accidentally enable them

## Credit
- [@thedax](https://github.com/thedax) for NX-ntpc, from which this project is forked.

## Functionality
- Change time by day/hour
- Contact a time server at http://ntp.org to set the time back to normal

## Building
Install [devkitA64 (along with libnx)](https://devkitpro.org/wiki/Getting_Started), and run `make` under the project directory.

## Disclaimer
This program changes NetworkSystemClock, which may cause a desync between console and servers. Use at your own risk! It is recommended that you only use the changed clock while offline, and change it back as soon as you are connected (either manually or using ntp.org server.)
