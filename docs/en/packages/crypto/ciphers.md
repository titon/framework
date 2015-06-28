# Ciphers #

A cryptography cipher is an algorithm that defines a set of steps for encryption and decryption. In regards to Titon, a `Titon\Crypto\Cipher` is a class that provides secure encryption and decryption through the use of initialization vectors (IV), message authentication codes (MAC), and unique keys (a password). Each `Cipher` class represents a specific cryptographic extension, either OpenSSL with `Titon\Crypto\OpenSslCipher` (preferred), or MCrypt with `Titon\Crypto\McryptCipher`.

`Cipher`s require a unique key, and a cipher algorithm and mode pair (known as a method in this package), which is passed as arguments to the constructor. The method defaults to `Cipher::AES_256_CBC`.

```hack
use Titon\Crypto\Cipher;
use Titon\Crypto\OpenSslCipher;

$key = md5('AUnique32MinimumCharacterHash');
$cipher = new OpenSslCipher($key, Cipher::AES_256_CBC);
```

## Keys ##

A key (also known as a password or salt) is a unique string used in the encryption and decryption process. A key must always be greater than or equal to 32 characters.

## Methods ##

A method is a combination of a cipher algorithm and a cipher mode. The primary cipher algorithm is [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) (compatible with Rijndael) with key sizes 128, 192, and 256. The secondary cipher algorithm is [Blowfish](https://en.wikipedia.org/wiki/Blowfish_%28cipher%29) with variable key sizes. Both algorithms support the following modes: cipher block chaining (CBC), cipher feedback (CFB), and output feedback (OFB).

Methods are represented with the following `Cipher` constants: `AES_128_CBC`, `AES_128_CFB`, `AES_128_OFB`, `AES_192_CBC`, `AES_192_CFB`, `AES_192_OFB`, `AES_256_CBC`, `AES_256_CFB`, `AES_256_OFB`, `BLOWFISH_CBC`, `BLOWFISH_CFB`, `BLOWFISH_OFB`. 

The `OpenSslCipher` also supports the following extra variants: `AES_128_CFB1`, `AES_128_CFB8`, `AES_192_CFB1`, `AES_192_CFB8`, `AES_256_CFB1`.

### Supported Methods ###

The static `getSupportedMethods()` method can be used to retrieve a list of supported cipher methods, which is passed as the 2nd argument to the constructor.

```hack
$methods = OpenSslCipher::getSupportedMethods(); // Vector<string>
```

## Encrypting & Decrypting ##

Once the `Cipher` has been initialized, a set of data can be encrypted using the `encrypt()` method. This method will accept any type of data that can be serialized. It will also return a package specific [payload](#payloads).

```hack
$payload = $cipher->encrypt($data);
```

To retrieve the original value from the payload, simply pass the payload to `decrypt()`.

```hack
$data = $cipher->decrypt($payload);
```

### Payloads ###

A payload is an encoded set of data that utilizes initialization vectors (IV) and message authentication codes (MAC) for authenticity, data integrity, and security. If a payload fails to decode, or the IV does not exist, or if the MAC hash does not match, the cipher will throw a `Titon\Crypto\Exception\InvalidPayloadException`.
