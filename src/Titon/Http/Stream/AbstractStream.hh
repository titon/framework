<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

use Psr\Http\Message\StreamInterface;
use Titon\Common\CacheMap;
use RuntimeException;

/**
 * Defines shared functionality for an HTTP resource stream.
 *
 * @package Titon\Http\Stream
 */
abstract class AbstractStream implements StreamInterface {

    /**
     * Cached meta data.
     *
     * @var \Titon\Common\CacheMap
     */
    protected CacheMap $cache = Map {};

    /**
     * The stream resource.
     *
     * @var resource
     */
    protected ?resource $stream;

    /**
     * Close the resource on destruction.
     */
    public function __destruct(): void {
        $this->close();
    }

    /**
     * {@inheritdoc}
     */
    public function __toString(): string {
        return $this->getContents();
    }

    /**
     * Build a meta cache for the current stream.
     *
     * @return $this
     */
    public function buildCache(): this {
        $cache = stream_get_meta_data($this->getStream());
        $cache['local'] = stream_is_local($this->getStream());

        $mode = str_replace('b', '', $cache['mode']);

        $cache['readable'] = !in_array($mode, ['w', 'a', 'x', 'c']);
        $cache['writable'] = ($mode !== 'r');

        $this->cache = new Map($cache);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function close(): bool {
        if (is_resource($this->getStream()) && fclose($this->getStream())) {
            $this->cache['readable'] = false;
            $this->cache['writable'] = false;

            return true;
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function detach(): ?resource {
        $resource = $this->stream;

        $this->stream = null;

        return $resource;
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
     * @return \Titon\Common\CacheMap
     */
    public function getCache(): CacheMap {
        return $this->cache;
    }

    /**
     * {@inheritdoc}
     */
    public function getContents($maxLength = -1): string {
        if (!$this->isReadable() || (!$this->isSeekable() && $this->eof())) {
            throw new RuntimeException('Stream is not readable');
        }

        // Save cursor position before reading
        $tell = $this->tell();

        $buffer = stream_get_contents($this->getStream(), $maxLength, 0);

        // Reset cursor position
        $this->seek($tell);

        if ($buffer === false) {
            throw new RuntimeException('Failed to read stream contents');
        }

        return $buffer;
    }

    /**
     * {@inheritdoc}
     */
    public function getMetadata($key = null): mixed {
        if ($key === null) {
            return $this->cache;
        }

        return $this->cache->get($key);
    }

    /**
     * Return the read/write mode used on the stream.
     *
     * @return string
     */
    public function getMode(): string {
        return (string) $this->cache['mode'];
    }

    /**
     * {@inheritdoc}
     */
    public function getSize(): int {
        if ($this->isLocal()) {
            // UNSAFE
            // HHVM only has the 1st argument
            clearstatcache(true, $this->cache['uri']);
        }

        if ($this->cache['wrapper_type'] !== 'PHP') {
            $stat = fstat($this->getStream());

            if (array_key_exists('size', $stat)) {
                return (int) $stat['size'];
            }
        }

        return (int) mb_strlen($this->getContents());
    }

    /**
     * Return the raw resource stream.
     *
     * @return resource
     */
    public function getStream(): ?resource {
        return $this->stream;
    }

    /**
     * Alias for eof().
     *
     * @return bool
     */
    public function isConsumed(): bool {
        return $this->eof();
    }

    /**
     * Return true if the stream is a local file.
     *
     * @return bool
     */
    public function isLocal(): bool {
        return (bool) $this->cache['local'];
    }

    /**
     * {@inheritdoc}
     */
    public function isReadable(): bool {
        return (bool) $this->cache['readable'];
    }

    /**
     * Return true if the stream can be re-read once the EOF has been reached.
     *
     * @return bool
     */
    public function isRepeatable(): bool {
        return $this->isReadable() && $this->isSeekable();
    }

    /**
     * {@inheritdoc}
     */
    public function isSeekable(): bool {
        return (bool) $this->cache['seekable'];
    }

    /**
     * {@inheritdoc}
     */
    public function isWritable(): bool {
        return (bool) $this->cache['writable'];
    }

    /**
     * {@inheritdoc}
     */
    public function read($length): string {
        if ($this->isReadable() && ($buffer = fread($this->getStream(), $length)) !== false) {
            return $buffer;
        }

        throw new RuntimeException('Failed to read from stream');
    }

    /**
     * Rewind the pointer to the beginning.
     *
     * @return bool
     */
    public function rewind(): bool {
        if ($this->isSeekable()) {
            return $this->seek(0);
        }

        throw new RuntimeException('Cannot rewind as stream is not seekable');
    }

    /**
     * {@inheritdoc}
     */
    public function seek($offset, $whence = SEEK_SET): bool {
        return $this->isSeekable() ? (fseek($this->getStream(), $offset, $whence) === 0) : false;
    }

    /**
     * Set a resource as a stream and generate a new meta data cache.
     *
     * @param resource $stream
     * @return $this
     */
    public function setStream(resource $stream): this {
        $this->stream = $stream;
        $this->buildCache();

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function tell(): int {
        $tell = ftell($this->getStream());

        if ($tell === false) {
            throw new RuntimeException('Failed to find stream position');
        }

        return $tell;
    }

    /**
     * {@inheritdoc}
     */
    public function write($string): int {
        if ($this->isWritable() && ($write = fwrite($this->getStream(), $string)) !== false) {
            return $write;
        }

        throw new RuntimeException('Failed to write to stream');
    }

}
