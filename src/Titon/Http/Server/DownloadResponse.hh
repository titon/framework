<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Common\Exception\MissingFileException;
use Titon\Http\Exception\InvalidFileException;
use Titon\Http\Exception\MalformedRequestException;
use Titon\Http\StatusCode;
use Titon\Http\Stream\FileStream;
use Titon\Utility\MimeType;
use Titon\Utility\Path;
use Titon\Utility\Exception\InvalidExtensionException;

/**
 * Force a file download by passing in a file path and setting all appropriate HTTP headers.
 *
 * @package Titon\Http\Server
 */
class DownloadResponse extends Response {
//
//    /**
//     * Path to the file.
//     *
//     * @var string
//     */
//    protected string $path = '';
//
//    /**
//     * Set the path of a file to output in the response.
//     *
//     * @param string $path
//     * @param int $status
//     * @throws \Titon\Common\Exception\MissingFileException
//     * @throws \Titon\Http\Exception\InvalidFileException
//     */
//    public function __construct(string $path, int $status = StatusCode::OK) {
//        $this->path = $path;
//
//        parent::__construct(new FileStream($path), $status);
//
//        $this->contentDisposition(basename($path));
//    }
//
//    /**
//     * Return the file path.
//     *
//     * @return string
//     */
//    public function getPath(): string {
//        return $this->path;
//    }
//
//    /**
//     * Set appropriate file range headers.
//     *
//     * @param string $path
//     * @return $this
//     */
//    public function setFileRange(string $path): this {
//        $request = $this->getRequest();
//
//        if (!$request) {
//            throw new MalformedRequestException('An incoming request is missing.');
//        }
//
//        if ($request->hasHeader('If-Range') || $request->getHeader('If-Range') !== $request->getHeader('ETag')) {
//            return $this;
//        }
//
//        list($start, $end) = explode('-', explode('=', $request->getHeader('Range'))[1]);
//
//        $size = filesize($path);
//        $end = ($end === '') ? $size - 1 : (int) $end;
//        $start = ($start === '') ? 0 : (int) $start;
//        $length = $end - $start + 1;
//
//        if ($start < 0 || $end > $size || $start > $size || $start > $end) {
//            $this->statusCode(StatusCode::REQUESTED_RANGE_NOT_SATISFIABLE);
//
//        } else {
//            $this
//                ->statusCode(StatusCode::PARTIAL_CONTENT)
//                ->contentLength($length)
//                ->contentRange($start, $end, $size);
//        }
//
//        return $this;
//    }
//
//    /**
//     * Validate the URL before sending.
//     *
//     * @return string
//     */
//    public function send(): string {
//        $path = $this->getPath();
//
//        try {
//            $contentType = MimeType::getTypeByExt(Path::ext($path));
//        } catch (InvalidExtensionException $e) {
//            $contentType = 'application/octet-stream';
//        }
//
//        $this
//            ->contentType($contentType)
//            ->acceptRanges()
//            ->setHeader('Content-Transfer-Encoding', 'binary')
//            ->setBody(new FileStream($path));
//
//        if ($this->getRequest()?->hasHeader('Range')) {
//            $this->setFileRange($path);
//        } else {
//            $this->contentLength(filesize($path));
//        }
//
//        return parent::send();
//    }
//
}
