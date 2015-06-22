# Ciphers #

A cryptography cipher is a type of algorithm that defines a set of steps for encryption and decryption. In regards to Titon, a `Titon\Crypto\Cipher` is a class that provides secure encryption and decryption through the user of initialization vectors (IV) and unique keys (a password). Each `Cipher` class represents a specific cryptographic extension, either OpenSSL with `Titon\Crypto\OpenSslCipher` (preferred), or MCrypt with `Titon\Crypto\McryptCipher`.

```hack
use Titon\Crypto\Cipher;
use Titon\Crypto\OpenSslCipher;

$cipher = new OpenSslCipher($key, Cipher::AES_256_CBC);
```

## Methods & Modes ##

The primary cipher method is [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) (also known as Rijndael) with key sizes 129, 192, and 256. The secondary cipher method is [Blowfish](https://en.wikipedia.org/wiki/Blowfish_%28cipher%29) with variable key sizes.

Both methods support the following modes: cipher block chaining (CBC), cipher feedback (CFB), and output feedback (OFB).

Method and mode pairs are represented with the following `Cipher` constants: `AES_128_CBC`, `AES_128_CFB`, `AES_128_OFB`, `AES_192_CBC`, `AES_192_CFB`, `AES_192_OFB`, `AES_256_CBC`, `AES_256_CFB`, `AES_256_OFB`, `BLOWFISH_CBC`, `BLOWFISH_CFB`, `BLOWFISH_OFB`. The `OpenSslCipher` also supports the following extra variants: `AES_128_CFB1`, `AES_128_CFB8`, `AES_192_CFB1`, `AES_192_CFB8`, `AES_256_CFB1`.

The static `getSupportedMethods()` method can be used to retrieve a list of supported method and mode pairs, which can be passed as the 2nd argument to the constructor.

```hack
$methods = OpenSslCipher::getSupportedMethods(); // Vector<string>
```

## Keys ##

A key (also known as a password or salt) is a unique string used in the encryption and decryption process. A key must always be greater than or equal to 32 characters.
