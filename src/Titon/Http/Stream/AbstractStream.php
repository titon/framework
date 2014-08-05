<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

use Psr\Http\Message\StreamInterface;

/**
 * Defines shared functionality for an HTTP resource stream.
 *
 * @package Titon\Http\Stream
 */
class AbstractStream implements StreamInterface {

    /**
     * Cached meta data.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_cache = Map {};

    /**
     * The stream resource.
     *
     * @type resource
     */
    protected ?resource $_stream;

    /**
     * Close the resource on destruction.
     */
    public function __destruct() {
        $this->close();
    }

    /**
     * {@inheritdoc}
     */
    public function __toString(): ?string {
        return $this->getContents();
    }

    /**
     * Build a meta cache for the current stream.
     *
     * @return $this
     */
    public function buildCache(): this {
        $cache = new Map(stream_get_meta_data($this->getStream()));
        $cache['readable'] = true;
        $cache['writable'] = true;

        $this->_cache = $cache;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function close(): bool {
        if (is_resource($this->getStream()) && fclose($this->getStream())) {
            $this->_cache['readable'] = false;
            $this->_cache['writable'] = false;

            return true;
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function detach(): bool {
        $this->_stream = null;

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function eof(): bool {
        return feof($this->getStream());
    }

    /**
     * Return the cached meta data.
     *
     * @return Map<string, mixed>
     */
    public function getCache(): Map<string, mixed> {
        return $this->_cache;
    }

    /**
     * {@inheritdoc}
     */
    public function getContents($maxLength = -1): ?string {
        if (!$this->isReadable() || (!$this->isSeekable() && $this->eof())) {
            return '';
        }

        $buffer = stream_get_contents($this->getStream(), $maxLength, 0);

        return ($buffer === false) ? null : $buffer;
    }

    /**
     * {@inheritdoc}
     */
    public function getSize(): int {
        $stat = fstat($this->getStream());

        if (isset($stat['size'])) {
            return $stat['size'];
        }

        return mb_strlen($this->getContents());
    }

    /**
     * Return the raw resource stream.
     *
     * @return resource
     */
    public function getStream(): ?resource {
        return $this->_stream;
    }

    /**
     * {@inheritdoc}
     */
    public function isReadable(): bool {
        return $this->_cache['readable'];
    }

    /**
     * {@inheritdoc}
     */
    public function isSeekable(): bool {
        return $this->_cache['seekable'];
    }

    /**
     * {@inheritdoc}
     */
    public function isWritable(): bool {
        return $this->_cache['writable'];
    }

    /**
     * {@inheritdoc}
     */
    public function read($length): ?string {
        if (!$this->cache['readable']) {
            return null;
        }

        $buffer = $length ? fgets($this->getStream(), $length) : fgets($this->getStream());

        return ($buffer === false) ? null : $buffer;
    }

    /**
     * {@inheritdoc}
     */
    public function seek($offset, $whence = SEEK_SET): bool {
        return $this->cache['seekable'] ? (fseek($this->getStream(), $offset, $whence) === 0) : false;
    }

    /**
     * Set a resource as a stream and generate a new meta data cache.
     *
     * @param resource $stream
     * @return $this
     */
    public function setStream(resource $stream): this {
        $this->_stream = $stream;
        $this->buildCache();

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function tell(): ?int {
        $tell = ftell($this->getStream());

        return ($tell === false) ? null : $tell;
    }

    /**
     * {@inheritdoc}
     */
    public function write($string): ?int {
        $write = fwrite($this->getStream(), $string);

        return ($write === false) ? null : $write;
    }

}