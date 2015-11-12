<?hh // partial
// Because of PSR7
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Stream;

use Psr\Http\Message\StreamInterface;
use Titon\Http\MetaDataMap;
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
     * @var \Titon\Http\MetaDataMap
     */
    protected MetaDataMap $metadata = Map {};

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
     * Build a meta metadata for the current stream.
     *
     * @return $this
     */
    public function buildMetaData(): this {
        $metadata = stream_get_meta_data($this->getStream());
        $metadata['local'] = stream_is_local($this->getStream());

        $mode = str_replace('b', '', $metadata['mode']);

        $metadata['readable'] = !in_array($mode, ['w', 'a', 'x', 'c']);
        $metadata['writable'] = ($mode !== 'r');

        $this->metadata = new Map($metadata);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function close(): void {
        $stream = $this->getStream();

        if (is_resource($stream) && fclose($stream)) {
            $this->detach();
        }
    }

    /**
     * {@inheritdoc}
     */
    public function detach(): ?resource {
        $resource = $this->stream;

        $this->stream = null;
        $this->metadata['readable'] = false;
        $this->metadata['writable'] = false;
        $this->metadata['seekable'] = false;

        return $resource;
    }

    /**
     * {@inheritdoc}
     */
    public function eof(): bool {
        return feof($this->getStream());
    }

    /**
     * {@inheritdoc}
     */
    public function getContents(): string {
        if (!$this->isReadable() || (!$this->isSeekable() && $this->eof())) {
            throw new RuntimeException('Stream is not readable');
        }

        // Save cursor position before reading
        $tell = $this->tell();
        $buffer = stream_get_contents($this->getStream(), -1, 0);

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
            return $this->metadata->toArray();
        }

        return $this->metadata->get($key);
    }

    /**
     * Return the read/write mode used on the stream.
     *
     * @return string
     */
    public function getMode(): string {
        return (string) $this->metadata['mode'];
    }

    /**
     * {@inheritdoc}
     */
    public function getSize(): int {
        if ($this->isLocal()) {
            // UNSAFE
            // HHVM only has the 1st argument
            clearstatcache(true, $this->metadata['uri']);
        }

        if ($this->metadata['wrapper_type'] !== 'PHP') {
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
        return (bool) $this->metadata['local'];
    }

    /**
     * {@inheritdoc}
     */
    public function isReadable(): bool {
        return (bool) $this->metadata['readable'];
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
        return (bool) $this->metadata['seekable'];
    }

    /**
     * {@inheritdoc}
     */
    public function isWritable(): bool {
        return (bool) $this->metadata['writable'];
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
     * {@inheritdoc}
     */
    public function rewind(): void {
        $this->seek(0);
    }

    /**
     * {@inheritdoc}
     */
    public function seek($offset, $whence = SEEK_SET): void {
        if ($this->isSeekable() && fseek($this->getStream(), $offset, $whence) !== 0) {
            throw new RuntimeException('Failed to seek on stream');
        }
    }

    /**
     * Set a resource as a stream and generate a new meta data cache.
     *
     * @param resource $stream
     * @return $this
     */
    public function setStream(resource $stream): this {
        $this->stream = $stream;
        $this->buildMetaData();

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
