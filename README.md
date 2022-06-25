Program to embed and extract files from .bmp with different steganography strategies: LSB1, LSB4 and LSBI.

# Authors
- [Agustín Naso]
- [Gaston De Schant]
- [Brittany Lin]

# Requirements
- [OpenSSL]
- [C compiler]

# Compilation

```bash
make all
```

# Execution

## Embed

```bash
./stegobmp -embed -in file -p carrier -out output_file -steg stego_algorithm [-a encryption_algorithm] [-m encryption_mode] [-pass password]
```

## Extract

```bash
./stegobmp -extract -p input_file -out output_file -steg stego_algorithm [-a encryption_algorithm] [-m encryption_mode] [-pass password]
```

## Arguments

* **-in**: file to hide
* **-p**: carrier file
* **-out**: output file
* **-steg**: steganography algorithm
  * LSB1
  * LSB4
  * LSBI
* **-a**: encryption algorithm
  * aes128
  * aes192
  * aes256
  * des
* **-m**: encryption mode
  * ecb
  * cfb
  * ofb
  * cbc
* **-pass**: encryption password