<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

class ResponseSender {

    /**
     * {@inheritdoc}
     */
    public function send(): string {
        $body = $this->getBody();
        $contents = (string) $body?->getContents();

        // Create an MD5 digest?
        if ($contents && $this->md5) {
            $this->withHeader('Content-MD5', base64_encode(pack('H*', md5($contents))));
        }

        // Return while in debug
        if ($this->isDebugging()) {
            return $contents;
        }

        $this->sendHeaders();
        $this->sendBody();

        if (function_exists('fastcgi_finish_request')) {
            fastcgi_finish_request();
        }

        return $contents;
    }

    /**
     * Output the body by echoing its contents.
     * If a buffer is set, chunk the body into parts.
     *
     * @return $this
     */
    public function sendBody(): this {
        $body = $this->getBody();

        if ($body) {
            $chunks = str_split($body->getContents(), 8192);

            foreach ($chunks as $chunk) {
                echo $chunk;
            }
        }

        return $this;
    }

    /**
     * Output headers and cookies defined for the current response.
     *
     * @return $this
     */
    public function sendHeaders(): this {
        if (headers_sent()) {
            return $this;
        }

        $status = $this->getStatusCode();

        header(sprintf('HTTP/%s %s %s', $this->getProtocolVersion(), $status, $this->getReasonPhrase()), true, $status);

        foreach ($this->getHeaders() as $header => $values) {
            foreach ($values as $value) {
                header($header . ': ' . $value, false, $status);
            }
        }

        return $this;
    }
}
