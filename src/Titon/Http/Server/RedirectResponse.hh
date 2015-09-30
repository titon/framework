<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\Exception\MalformedResponseException;
use Titon\Http\HeaderMap;
use Titon\Http\StatusCode;
use Titon\Http\Stream\MemoryStream;
use Titon\Utility\Sanitize;

/**
 * A response used for redirecting the request to a new URL.
 *
 * @package Titon\Http\Server
 */
class RedirectResponse extends Response {

    /**
     * Set the redirection URL and status code.
     *
     * @param string $url
     * @param int $status
     * @param \Titon\Http\HeaderMap $headers
     * @throws \Titon\Http\Exception\MalformedResponseException
     */
    public function __construct(string $url, int $status = StatusCode::FOUND, HeaderMap $headers = Map {}) {
        if (!$url) {
            throw new MalformedResponseException('Redirect URL cannot be empty');
        }

        $type = $this->validateContentType('html');
        $body = new MemoryStream(sprintf('<!DOCTYPE html>
            <html>
            <head>
                <meta http-equiv="Content-Type" content="%s">
                <meta http-equiv="refresh" content="0; url=%s">
                <title>Redirecting</title>
            </head>
            <body></body>
            </html>', $type, Sanitize::escape($url)));

        $headers['Location'] = [$url];
        $headers['Content-Type'] = [$type];

        parent::__construct($body, $status, $headers);
    }

}
