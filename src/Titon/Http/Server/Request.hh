<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\ServerRequestInterface;
use Psr\Http\Message\StreamInterface;
use Psr\Http\Message\UriInterface;
use Titon\Http\AbstractRequest;
use Titon\Http\Bag\CookieBag;
use Titon\Http\Bag\ParameterBag;
use Titon\Http\HeaderMap;
use Titon\Utility\State\GlobalMap;

/**
 * The Request object is the primary source of data and state management for the environment.
 * It extracts and cleans the GET, POST and FILES data from the current HTTP request.
 *
 * @package Titon\Http\Server
 */
class Request extends AbstractRequest implements ServerRequestInterface {

    /**
     * Custom attributes for the request.
     *
     * @var \Titon\Http\Bag\ParameterBag
     */
    protected ParameterBag $attributes;

    /**
     * COOKIE data for the request.
     *
     * @var \Titon\Http\Bag\CookieBag
     */
    protected CookieBag $cookies;

    /**
     * FILES data for the request.
     *
     * @var \Titon\Http\Bag\ParameterBag
     */
    protected ParameterBag $files;

    /**
     * POST data for the request.
     *
     * @var \Titon\Http\Bag\ParameterBag
     */
    protected ParameterBag $post;

    /**
     * GET data for the request.
     *
     * @var \Titon\Http\Bag\ParameterBag
     */
    protected ParameterBag $query;

    /**
     * SERVER environment variables.
     *
     * @var \Titon\Http\Bag\ParameterBag
     */
    protected ParameterBag $server;

    /**
     * When enabled, will use applicable HTTP headers set by proxies.
     *
     * @var bool
     */
    protected bool $trustProxies = true;

    /**
     * Setup the request and instantiate all bags.
     *
     * @param \Titon\Utility\State\GlobalMap $server
     * @param \Psr\Http\Message\UriInterface $uri
     * @param string $method
     * @param \Psr\Http\Message\StreamInterface $body
     * @param \Titon\Http\HeaderMap $headers
     */
    public function __construct(GlobalMap $server, UriInterface $uri, string $method = 'GET', ?StreamInterface $body = null, HeaderMap $headers = Map {}) {
        parent::__construct($uri, $method, $body, $headers);

        $this->attributes = new ParameterBag();
        $this->cookies = new CookieBag();
        $this->files = new ParameterBag();
        $this->post = new ParameterBag();
        $this->query = new ParameterBag();
        $this->server = new ParameterBag($server);
    }

    /**
     * Clone the bags.
     */
    public function __clone(): void {
        $this->attributes = clone $this->attributes;
        $this->body = clone $this->body;
        $this->cookies = clone $this->cookies;
        $this->files = clone $this->files;
        $this->headers = clone $this->headers;
        $this->post = clone $this->post;
        $this->query = clone $this->query;
        $this->server = clone $this->server;
    }

    /**
     * Do not trust the headers from proxies.
     *
     * @return $this;
     */
    public function dontTrustProxies(): this {
        $this->trustProxies = false;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getAttribute($key, $default = null): mixed {
        return $this->attributes->get($key, $default);
    }

    /**
     * {@inheritdoc}
     */
    public function getAttributes(): array<string, mixed> {
        return $this->attributes->all()->toArray();
    }

    /**
     * Get the IP address of the client, the correct way.
     * If trust proxies is enabled, use appropriate headers.
     *
     * @return string
     */
    public function getClientIP(): string {
        $headers = ['REMOTE_ADDR', 'HTTP_CLIENT_IP'];
        $ip = '';

        if ($this->isTrustingProxies()) {
            array_unshift($headers, 'HTTP_X_FORWARDED_FOR');
        }

        foreach ($headers as $header) {
            if ($ip = (string) $this->server->get($header)) {
                if (strpos($ip, ',') !== false) {
                    $ip = trim(explode(',', $ip)[0]);
                }

                break;
            }
        }

        return (string) $ip;
    }

    /**
     * {@inheritdoc}
     */
    public function getCookieParams(): array<string, mixed> {
        return $this->cookies->all()->toArray();
    }

    /**
     * {@inheritdoc}
     */
    public function getHost(): string {
        $headers = ['HTTP_HOST', 'SERVER_NAME', 'SERVER_ADDR'];
        $host = '';

        if ($this->isTrustingProxies()) {
            array_unshift($headers, 'HTTP_X_FORWARDED_HOST');
        }

        foreach ($headers as $header) {
            if ($host = $this->server->get($header)) {
                break;
            }
        }

        return preg_replace('/:\d+$/', '', trim(strtolower((string) $host)));
    }

    /**
     * {@inheritdoc}
     */
    public function getParsedBody(): array<string, mixed> {
        return $this->post->toArray();
    }

    /**
     * {@inheritdoc}
     */
    public function getProtocolVersion(): string {
        if ($protocol = parent::getProtocolVersion()) {
            return $protocol;
        }

        return (string) $this->server->get('SERVER_PROTOCOL', '1.1');
    }

    /**
     * Return the valid port based on possible server environment variables and HTTP headers.
     * If trust proxies is enabled, use appropriate headers.
     *
     * @return int
     */
    public function getPort(): int {
        if ($this->isTrustingProxies()) {
            if ($port = $this->server->get('HTTP_X_FORWARDED_PORT')) {
                return (int) $port;
            }

            if ($this->server->get('HTTP_X_FORWARDED_PROTO') === 'https') {
                return 443;
            }
        }

        if ($host = (string) $this->server->get('HTTP_HOST')) {
            if (strpos($host, ':') !== false) {
                return (int) explode(':', $host)[1];
            }

            return ($this->getScheme() === 'https') ? 443 : 80;
        }

        if ($port = $this->getUri()->getPort()) {
            return $port;
        }

        return (int) $this->server->get('SERVER_PORT');
    }

    /**
     * {@inheritdoc}
     */
    public function getQueryParams(): array<string, mixed> {
        return $this->query->all()->toArray();
    }

    /**
     * Get the referring URL. Will strip the hostname if it comes from the same domain.
     *
     * @return string
     */
    public function getReferrer(): string {
        $referrer = $this->server->get('HTTP_REFERER');

        if (!$referrer) {
            return '/';
        }

        $host = (string) $this->server->get('HTTP_HOST');

        if (strpos($referrer, $host) !== false) {
            $referrer = str_replace($this->getScheme() . '://' . $host, '', $referrer);
        }

        return trim($referrer);
    }

    /**
     * {@inheritdoc}
     */
    public function getScheme(): string {
        return $this->isSecure() ? 'https' : 'http';
    }

    /**
     * Get the IP address of the current server.
     *
     * @return string
     */
    public function getServerIP(): string {
        return (string) $this->server->get('SERVER_ADDR');
    }

    /**
     * {@inheritdoc}
     */
    public function getServerParams(): array<string, mixed> {
        return $this->server->toArray();
    }

    /**
     * {@inheritdoc}
     */
    public function getUploadedFiles(): array<string, mixed> {
        return $this->files->toArray();
    }

    /**
     * Grabs information about the browser, the OS, and other client information.
     *
     * @return string
     */
    public function getUserAgent(): string {
        return (string) $this->server->get('HTTP_USER_AGENT') ?: '';
    }

    /**
     * Returns true if the page was requested with AJAX.
     *
     * @return bool
     */
    public function isAJAX(): bool {
        return (strtolower($this->server->get('HTTP_X_REQUESTED_WITH')) === 'xmlhttprequest');
    }

    /**
     * Returns true if the interface environment is CGI.
     *
     * @return bool
     */
    public function isCGI(): bool {
        return (substr(PHP_SAPI, 0, 3) === 'cgi');
    }

    /**
     * Returns true if the interface environment is CLI (command line).
     *
     * @return bool
     */
    public function isCLI(): bool {
        return (substr(PHP_SAPI, 0, 3) === 'cli');
    }

    /**
     * Returns true if the page was requested with a DELETE method.
     *
     * @return bool
     */
    public function isDelete(): bool {
        return $this->isMethod('delete');
    }

    /**
     * Returns true if the page was requested through Flash.
     *
     * @return bool
     */
    public function isFlash(): bool {
        return (bool) preg_match('/^(shockwave|adobe) flash/i', $this->getUserAgent());
    }

    /**
     * Returns true if the page was requested with a GET method.
     *
     * @return bool
     */
    public function isGet(): bool {
        return $this->isMethod('get');
    }

    /**
     * Returns true if the interface environment is IIS.
     *
     * @return bool
     */
    public function isIIS(): bool {
        return (substr(PHP_SAPI, 0, 5) === 'isapi');
    }

    /**
     * Primary container function for all method type checking. Returns true if the current request method matches the given argument.
     *
     * @param string $type
     * @return bool
     */
    public function isMethod(string $type): bool {
        return (strtoupper($type) === $this->getMethod());
    }

    /**
     * Returns true if the page was requested with a mobile device.
     *
     * @link http://detectmobilebrowsers.com/
     *
     * @return bool
     */
    public function isMobile(): bool {
        $mobiles  = '(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|';
        $mobiles .= 'ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|mobile.+firefox|netfront|opera m(ob|in)i|palm( os)?|';
        $mobiles .= 'phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|';
        $mobiles .= 'windows (ce|phone)|xda|xiino';

        $versions  = '1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|';
        $versions .= 'an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|';
        $versions .= 'br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|';
        $versions .= 'dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|';
        $versions .= 'ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|';
        $versions .= 'hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|';
        $versions .= 'ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |';
        $versions .= 'kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|';
        $versions .= 'mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|';
        $versions .= 'mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|';
        $versions .= 'nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|';
        $versions .= 'po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|';
        $versions .= 'ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|';
        $versions .= 'shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|';
        $versions .= 't6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|';
        $versions .= 'up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|';
        $versions .= '81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-';

        $agent = $this->getUserAgent();

        return (preg_match('/' . $mobiles . '/i', $agent) || preg_match('/' . $versions . '/i', substr($agent, 0, 4)));
    }

    /**
     * Returns true if the page was requested with a POST method.
     *
     * @return bool
     */
    public function isPost(): bool {
        return $this->isMethod('post');
    }

    /**
     * Returns true if the page was requested with a PUT method.
     *
     * @return bool
     */
    public function isPut(): bool {
        return $this->isMethod('put');
    }

    /**
     * Is the page being requested through a secure connection.
     *
     * @return bool
     */
    public function isSecure(): bool {
        if ($this->isTrustingProxies()) {
            if ($scheme = $this->server->get('HTTP_X_FORWARDED_PROTO')) {
                return ($scheme === 'https');
            }
        }

        return (strtolower($this->server->get('HTTPS')) === 'on' || $this->server->get('SERVER_PORT') == 443);
    }

    /**
     * Return true if we should trust proxies headers.
     *
     * @return bool
     */
    public function isTrustingProxies(): bool {
        return $this->trustProxies;
    }

    /**
     * Trust the headers from proxies.
     *
     * @return $this;
     */
    public function trustProxies(): this {
        $this->trustProxies = true;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withCookieParams(array $cookies): this {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withQueryParams(array $query): this {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withUploadedFiles(array $files): this {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withParsedBody($data): this {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withAttribute($name, $value): this {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function withoutAttribute($name): this {
        return $this;
    }

}

