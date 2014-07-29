<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * The body of a HTTP request.
 *
 * Based on the PHP-FIG HTTP spec. https://github.com/php-fig/http-message/blob/master/src/StreamInterface.php
 *
 * @package Titon\Http
 */
interface Stream {

    /**
     * Reads all data from the stream into a string, from the beginning to end.
     *
     * This method MUST attempt to seek to the beginning of the stream before
     * reading data and read the stream until the end is reached.
     *
     * Warning: This could attempt to load a large amount of data into memory.
     *
     * @return string
     */
    public function __toString(): string;

    /**
     * Closes the stream and any underlying resources.
     *
     * @return $this
     */
    public function close(): this;

    /**
     * Separates any underlying resources from the stream.
     *
     * After the stream has been detached, the stream is in an unusable state.
     *
     * @return $this
     */
    public function detach(): this;

    /**
     * Get the size of the stream if known
     *
     * @return int|null Returns the size in bytes if known, or null if unknown
     */
    public function getSize(): ?int;

    /**
     * Returns the current position of the file read/write pointer
     *
     * @return int|null Position of the file pointer or null on error
     */
    public function tell(): ?int;

    /**
     * Returns true if the stream is at the end of the stream.
     *
     * @return bool
     */
    public function eof(): bool;

    /**
     * Returns whether or not the stream is seekable
     *
     * @return bool
     */
    public function isSeekable(): bool;

    /**
     * Seek to a position in the stream
     *
     * @param int $offset Stream offset
     * @param int $whence Specifies how the cursor position will be calculated
     * based on the seek offset. Valid values are identical
     * to the built-in PHP $whence values for `fseek()`.
     * SEEK_SET: Set position equal to offset bytes
     * SEEK_CUR: Set position to current location plus offset
     * SEEK_END: Set position to end-of-stream plus offset
     *
     * @return bool Returns TRUE on success or FALSE on failure
     * @link http://www.php.net/manual/en/function.fseek.php
     */
    public function seek($offset, $whence = SEEK_SET): bool;

    /**
     * Returns whether or not the stream is writable
     *
     * @return bool
     */
    public function isWritable(): bool;

    /**
     * Write data to the stream.
     *
     * @param string $string The string that is to be written.
     *
     * @return int|null Returns the number of bytes written to the stream on
     * success or null on failure.
     */
    public function write($string): ?int;

    /**
     * Returns whether or not the stream is readable
     *
     * @return bool
     */
    public function isReadable(): bool;

    /**
     * Read data from the stream
     *
     * @param int $length Read up to $length bytes from the object and return
     * them. Fewer than $length bytes may be returned if
     * underlying stream call returns fewer bytes.
     *
     * @return string Returns the data read from the stream.
     */
    public function read($length): string;

    /**
     * Returns the remaining contents in a string, up to maxlength bytes.
     *
     * @param int $maxLength The maximum bytes to read. Defaults to -1 (read
     * all the remaining buffer).
     * @return string
     */
    public function getContents($maxLength = -1): string;

}