![RACFu Logo](logo.png)

A standardized JSON interface for RACF that enables seemless exploitation by programming languages that have a foreign language interface for C/C++ and native JSON support.

## Description

As automation becomes more and more prevalent, the need to manage the security environment programmaticaly increases. On z/OS that means managing a security product like the IBM **Resource Access Control Facility** _(RACF)_. RACF is the primary facility for managing identity, authority, and access control for z/OS. There are more than 50 callable services with assembler interfaces that are part of the RACF API. The complete set of interfaces can be found [here](http://publibz.boulder.ibm.com/epubs/pdf/ich2d112.pdf).

While there are a number of languages that can be used to manage RACF, _(from low level lnaguages like Assembler to higher level languages like REXX)_, the need to be able to easily exploit RACF management functions using existing indurstry standard programming languages and even programming languages that don't exist yet is paramount. The RACFu project is focused on making RACF management functions available to all programming languages that have native JSON support and a foreign language interface for C/C++. This will make it easier to pivot to new tools and programming languages as technology, skills, and business needs continue to evolve in the forseeable future.

## Getting Started

### Dependencies

* z/OS **2.4** or higher.
* **R_SecMgtOper (IRRSMO00)**: Security Management Operations.
  * More details about the authorizations required for **IRRSMO00** can be found [here](https://www.ibm.com/docs/en/zos/3.1.0?topic=operations-racf-authorization).
* **R_Admin (IRRSEQ00)**: RACF Administration API.
  * More details about the authorizations required for **IRRSEQ00** can be found [here](https://www.ibm.com/docs/en/zos/3.1.0?topic=api-racf-authorization).

### Installation

> :bulb: _Note: You can also [Download & Install RACFu from GitHub](https://github.com/ambitus/racfu/releases)_

```shell
python3 -m pip install racfu
```

## Help
* [GitHub Discussions](https://github.com/ambitus/racfu/discussions)

## Authors

* Joe Bostian: jbostian@ibm.com
* Frank De Gilio: degilio@us.ibm.com
* Leonard Carcaramo: lcarcaramo@ibm.com
* Elijah Swift: Elijah.Swift@ibm.com
