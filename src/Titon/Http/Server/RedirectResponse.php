<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Http\Exception\MalformedResponseException;
use Titon\Http\Http;
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
     * @param array $config
     */
    public function __construct($url, $status = Http::FOUND, array $config = []) {
        parent::__construct('', $status, $config);

        $this->headers->flush();

        $this
            ->date(time())
            ->location($url)
            ->statusCode($status);
    }

    /**
     * Validate the URL before sending.
     *
     * @return string
     * @throws \Titon\Http\Exception\MalformedResponseException
     */
    public function send() {
        $url = $this->getHeader('Location');

        if (!$url) {
            throw new MalformedResponseException('Redirect URL cannot be empty');
        }

        $this->setBody(sprintf('<!DOCTYPE html>
            <html>
            <head>
                <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
                <meta http-equiv="refresh" content="0; url=%1$s">
                <title>Redirecting to %1$s</title>
            </head>
            <body></body>
            </html>', Sanitize::escape($url)));

        return parent::send();
    }

}