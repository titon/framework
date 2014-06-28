<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\Exception\InvalidExtensionException;
use Titon\Http\Exception\InvalidFileException;
use Titon\Http\Http;
use Titon\Http\Mime;
use Titon\Utility\Path;

/**
 * Force a file download by passing in a file path and setting all appropriate HTTP headers.
 *
 * @package Titon\Http\Server
 */
class DownloadResponse extends Response {

    /**
     * Configuration.
     *
     * @type array {
     *      @type bool $autoEtag            Automatically set an ETag header
     *      @type bool $autoModified        Automatically set a last modified header based on file modified time
     *      @type string $dispositionName   Custom file name to use when forcing a download
     * }
     */
    protected $_config = [
        'autoEtag' => false,
        'autoModified' => true,
        'dispositionName' => ''
    ];

    /**
     * Path to the file.
     *
     * @type string
     */
    protected $_path;

    /**
     * Set the path of a file to output in the response.
     *
     * @param string $path
     * @param int $status
     * @param array $config
     * @throws \Titon\Http\Exception\InvalidFileException
     */
    public function __construct($path, $status = Http::OK, array $config = []) {
        parent::__construct('', $status, $config);

        if (!file_exists($path)) {
            throw new InvalidFileException(sprintf('File %s does not exist', basename($path)));

        } else if (!is_readable($path)) {
            throw new InvalidFileException(sprintf('File %s is not readable', basename($path)));
        }

        $this->_path = $path;
    }

    /**
     * Set appropriate file range headers.
     *
     * @param string $path
     * @return $this
     */
    public function setFileRange($path) {
        $request = $this->getRequest();

        if ($request->hasHeader('If-Range') || $request->getHeader('If-Range') !== $request->getHeader('ETag')) {
            return $this;
        }

        list($start, $end) = explode('-', explode('=', $request->getHeader('Range'))[1]);

        $size = filesize($path);
        $end = ($end === '') ? $size - 1 : (int) $end;
        $start = ($start === '') ? 0 : (int) $start;
        $length = $end - $start + 1;

        if ($start < 0 || $end > $size || $start > $size || $start > $end) {
            $this->statusCode(Http::REQUESTED_RANGE_NOT_SATISFIABLE);

        } else {
            $this
                ->statusCode(Http::PARTIAL_CONTENT)
                ->contentLength($length)
                ->contentRange($start, $end, $size);
        }

        return $this;
    }

    /**
     * Validate the URL before sending.
     *
     * @return string
     */
    public function send() {
        $path = $this->_path;

        try {
            $contentType = Mime::getTypeByExt(Path::ext($path));
        } catch (InvalidExtensionException $e) {
            $contentType = 'application/octet-stream';
        }

        $this
            ->contentDisposition($this->getConfig('dispositionName') ?: basename($path))
            ->contentType($contentType)
            ->acceptRanges()
            ->setHeader('Content-Transfer-Encoding', 'binary')
            ->setBody(file_get_contents($path));

        if ($this->getConfig('autoModified')) {
            $this->lastModified(filemtime($path));
        }

        if ($this->getConfig('autoEtag')) {
            $this->etag(sha1_file($path));
        }

        if ($this->getRequest()->hasHeader('Range')) {
            $this->setFileRange($path);
        } else {
            $this->contentLength(filesize($path));
        }

        return parent::send();
    }

}