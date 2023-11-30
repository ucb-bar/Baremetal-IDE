# Installing Baremetal-IDE

## 1. Prerequisites

Baremetal-IDE uses [Microsoft Visual Studio Code](https://code.visualstudio.com/) as the editor and host of the integrated development environment. Install Microsoft VS Code from [here](https://code.visualstudio.com/download).



## 2. Clone the Github Repository

### Method 1: Install it with Chipyard

Follow the Chipyard installation tutorial to set up Chipyard. The Baremetal-IDE environment is included as a submodule in Chipyard, located at `software/baremetal-ide`

{% embed url="https://chipyard.readthedocs.io/en/stable/Chipyard-Basics/Initial-Repo-Setup.html" %}



### Method 2: Install it as a standalone environment

```bash
git clone https://github.com/ucb-bar/Baremetal-IDE.git baremetal-ide
```



Execute the following commands to initialize the board support files.

```bash
cd baremetal-ide
git submodule init
git submodule update
```



## 3. Set up VS Code

In Microsoft Visual Studio Code, search and install the extension "Cortex-Debug".

<figure><img src="../.gitbook/assets/image (3).png" alt=""><figcaption></figcaption></figure>

