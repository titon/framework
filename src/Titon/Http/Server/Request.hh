<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Titon\Common\FactoryAware;
use Titon\Http\Cookie;
use Titon\Http\Message;
use Titon\Http\Bag\CookieBag;
use Titon\Http\Bag\ParameterBag;
use Titon\Http\Exception\InvalidMethodException;
use Titon\Http\Http;
use Titon\Http\Mime;
use Titon\Http\IncomingRequest;
use Titon\Utility\State\Cookie as CookieGlobal;
use Titon\Utility\State\Files;
use Titon\Utility\State\Get;
use Titon\Utility\State\GlobalMap;
use Titon\Utility\State\Post;
use Titon\Utility\State\Server;

type AcceptHeader = shape('value' => string, 'quality' => float);

/**
 * The Request object is the primary source of data and state management for the environment.
 * It extracts and cleans the GET, POST and FILES data from the current HTTP request.
 *
 * @package Titon\Http\Server
 */
<<__ConsistentConstruct>>
class Request extends Message implements IncomingRequest {
    use FactoryAware;

    /**
     * Custom attributes for the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public ParameterBag $attributes;

    /**
     * COOKIE data for the request.
     *
     * @type \Titon\Http\Bag\CookieBag
     */
    public CookieBag $cookies;

    /**
     * FILES data for the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public ParameterBag $files;

    /**
     * POST data for the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public ParameterBag $post;

    /**
     * GET data for the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public ParameterBag $query;

    /**
     * SERVER environment variables.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public ParameterBag $server;

    /**
     * The current type of request method.
     *
     * @type string
     */
    protected string $_method = '';

    /**
     * When enabled, will use applicable HTTP headers set by proxies.
     *
     * @type bool
     */
    protected bool $_trustProxies = true;

    /**
     * The current URL for the request.
     *
     * @type string
     */
    protected string $_url = '';

    /**
     * Load post data, query data, files data, cookies, server and environment settings.
     *
     * @param \Titon\Utility\State\GlobalMap $query
     * @param \Titon\Utility\State\GlobalMap $post
     * @param \Titon\Utility\State\GlobalMap $files
     * @param \Titon\Utility\State\GlobalMap $cookies
     * @param \Titon\Utility\State\GlobalMap $server
     */
    public function __construct(GlobalMap $query = Map {}, GlobalMap $post = Map {}, GlobalMap $files = Map {}, GlobalMap $cookies = Map {}, GlobalMap $server = Map {}) {
        parent::__construct();

        // Fix method overrides
        if ($post->contains('_method')) {
            $server['HTTP_X_METHOD_OVERRIDE'] = $post['_method'];
            $post->remove('_method');
        }

        // Create bags
        $this->attributes = new ParameterBag();
        $this->cookies = new CookieBag($cookies);
        $this->files = new ParameterBag($files);
        $this->post = new ParameterBag($post);
        $this->query = new ParameterBag($query);
        $this->server = new ParameterBag($server);

        // Extract headers from server
        $headers = Map {};

        foreach ($server as $key => $value) {
            if (substr($key, 0, 5) === 'HTTP_') {
                $key = substr($key, 5);

            } else if (!in_array($key, ['CONTENT_LENGTH', 'CONTENT_MD5', 'CONTENT_TYPE'])) {
                continue;
            }

            $headers[$key] = explode(',', $value);
        }

        $this->headers->add($headers);
    }

    /**
     * Clone the bags.
     */
    public function __clone(): void {
        $this->attributes = clone $this->attributes;
        $this->cookies = clone $this->cookies;
        $this->files = clone $this->files;
        $this->headers = clone $this->headers;
        $this->post = clone $this->post;
        $this->query = clone $this->query;
        $this->server = clone $this->server;
    }

    /**
     * Create a request object based off the super globals.
     *
     * @return $this
     */
    public static function createFromGlobals(): Request {
        return new static(Get::all(), Post::all(), Files::all(), CookieGlobal::all(), Server::all());
    }

    /**
     * Checks to see if the client accepts a certain content type, based on the Accept header.
     *
     * @uses Titon\Http\Mime
     *
     * @param string $type
     * @return \Titon\Http\AcceptHeader
     */
    public function accepts(mixed $type): ?AcceptHeader {
        if (is_array($type)) {
            $contentType = $type;
        } else if (strpos($type, '/') !== false) {
            $contentType = [$type];
        } else {
            $contentType = [Mime::getTypeByExt((string) $type)];
        }

        foreach ($this->_extractAcceptHeaders('Accept') as $accept) {
            foreach ($contentType as $cType) {
                if ($cType === $accept['value'] || $accept['value'] === '*/*') {
                    return $accept;

                // Wildcard matching
                } else if (strpos($accept['value'], '/*') && strpos($cType, trim($accept['value'], '*')) === 0) {
                    return $accept;
                }
            }
        }

        return null;
    }

    /**
     * Checks to see if the client accepts a certain charset, based on the Accept-Charset header.
     *
     * @param string $charset
     * @return \Titon\Http\AcceptHeader
     */
    public function acceptsCharset(string $charset): ?AcceptHeader {
        foreach ($this->_extractAcceptHeaders('Accept-Charset') as $accept) {
            if (strtolower($charset) === $accept['value'] || $accept['value'] === '*') {
                return $accept;
            }
        }

        return null;
    }

    /**
     * Checks to see if the client accepts a certain encoding, based on the Accept-Encoding header.
     *
     * @param string $encoding
     * @return \Titon\Http\AcceptHeader
     */
    public function acceptsEncoding(string $encoding): ?AcceptHeader {
        foreach ($this->_extractAcceptHeaders('Accept-Encoding') as $accept) {
            if (strtolower($encoding) === $accept['value'] || $accept['value'] === '*') {
                return $accept;
            }
        }

        return null;
    }

    /**
     * Checks to see if the client accepts a certain charset, based on the Accept-Language header.
     *
     * @param string $language
     * @return \Titon\Http\AcceptHeader
     */
    public function acceptsLanguage(string $language): ?AcceptHeader {
        foreach ($this->_extractAcceptHeaders('Accept-Language') as $accept) {
            if (strtolower($language) === $accept['value'] || $accept['value'] === '*') {
                return $accept;
            }
        }

        return null;
    }

    /**
     * Do not trust the headers from proxies.
     *
     * @return $this;
     */
    public function dontTrustProxies(): this {
        $this->_trustProxies = false;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getAttribute($attribute, $default = null): mixed {
        return $this->attributes->get($attribute, $default);
    }

    /**
     * {@inheritdoc}
     */
    public function getAttributes(): array<string, mixed> {
        return $this->attributes->all()->toArray();
    }

    /**
     * {@inheritdoc}
     */
    public function getBodyParams(): array<string, mixed> {
        return $this->post->toArray();
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
            if ($ip = $this->server->get($header)) {
                if (strpos($ip, ',') !== false) {
                    $ip = trim(explode(',', $ip)[0]);
                }

                break;
            }
        }

        return (string) $ip;
    }

    /**
     * Return an HTTP cookie as a Cookie class defined by key/name.
     *
     * @param string $key
     * @return \Titon\Http\Cookie
     */
    public function getCookie(string $key): ?Cookie {
        return $this->cookies->get($key);
    }

    /**
     * Return all HTTP cookies as Cookie classes.
     *
     * @return Map<string, Cookie>
     */
    public function getCookies(): Map<string, Cookie> {
        return $this->cookies->all();
    }

    /**
     * {@inheritdoc}
     */
    public function getCookieParams(): array<string, mixed> {
        $array = [];

        foreach ($this->getCookies() as $key => $cookie) {
            $array[$key] = $cookie->getDecryptedValue();
        }

        return $array;
    }

    /**
     * {@inheritdoc}
     */
    public function getFileParams(): array<string, mixed> {
        return $this->files->toArray();
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

        return preg_replace('/:\d+$/', '', trim(strtolower($host)));
    }

    /**
     * {@inheritdoc}
     */
    public function getMethod(): string {
        if (!$this->_method) {
            $method = strtoupper($this->server->get('REQUEST_METHOD', 'GET'));

            if ($method === 'POST') {
                if ($override = $this->server->get('HTTP_X_METHOD_OVERRIDE')) {
                    $method = strtoupper($override);
                }
            }

            $this->setMethod($method);
        }

        return $this->_method;
    }

    /**
     * {@inheritdoc}
     */
    public function getProtocolVersion(): string {
        return $this->server->get('SERVER_PROTOCOL', '1.1');
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

        if ($host = $this->server->get('HTTP_HOST')) {
            if (strpos($host, ':') !== false) {
                return (int) explode(':', $host)[1];
            }

            return ($this->getScheme() === 'https') ? 443 : 80;
        }

        return (int) $this->server->get('SERVER_PORT');
    }

    /**
     * {@inheritdoc}
     */
    public function getQueryParams(): array<string, mixed> {
        return $this->query->toArray();
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

        $host = $this->server->get('HTTP_HOST');

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
        return $this->server->get('SERVER_ADDR');
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
    public function getUrl(): string {
        if (!$this->_url) {
            $server = $this->server;
            $script = str_replace($server->get('DOCUMENT_ROOT'), '', $server->get('SCRIPT_FILENAME'));
            $base = dirname($script);
            $url = '/';

            // Proper URL defined by the web server
            if ($path = $server->get('PATH_INFO')) {
                $url = $path;

            // Strip off the query string if it exists
            } else if ($path = $server->get('REQUEST_URI')) {
                $url = explode('?', $path)[0];

            // Remove the base folder and index file
            } else if ($path = $server->get('PHP_SELF')) {
                $url = str_replace($script, '', $path);
            }

            if ($base !== '/' && $base !== '\\') {
                $url = $base . $url;
            }

            $this->setUrl($url);
        }

        return $this->_url;
    }

    /**
     * Grabs information about the browser, os and other client information.
     * Must have browscap installed for $explicit use.
     *
     * @link http://php.net/get_browser
     * @link http://php.net/manual/misc.configuration.php#ini.browscap
     *
     * @param bool $explicit
     * @return array|string
     */
    public function getUserAgent(bool $explicit = false): mixed {
        $agent = $this->server->get('HTTP_USER_AGENT');

        if ($explicit && function_exists('get_browser')) {
            $browser = get_browser($agent, true);

            return [
                'browser' => $browser['browser'],
                'version' => $browser['version'],
                'cookies' => $browser['cookies'],
                'agent' => $agent,
                'os' => $browser['platform']
            ];
        }

        return $agent;
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
        return (bool) preg_match('/^(shockwave|adobe) flash/i', $this->getUserAgent(false));
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

        $agent = $this->getUserAgent(false);

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
        return $this->_trustProxies;
    }

    /**
     * {@inheritdoc}
     */
    public function setAttribute($attribute, $value): this {
        $this->attributes->set($attribute, $value);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setAttributes(array $attributes): this {
        foreach ($attributes as $attribute => $value) {
            $this->attributes->set($attribute, $value);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setMethod($method): this {
        $method = strtoupper($method);

        if (!in_array($method, Http::getMethodTypes())) {
            throw new InvalidMethodException(sprintf('Invalid method %s', $method));
        }

        $this->_method = $method;
        $this->server->set('REQUEST_METHOD', $method);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setUrl($url): this {
        $this->_url = $url;

        return $this;
    }

    /**
     * Trust the headers from proxies.
     *
     * @return $this;
     */
    public function trustProxies(): this {
        $this->_trustProxies = true;

        return $this;
    }

    /**
     * Lazy loading functionality for extracting Accept header information and parsing it.
     *
     * @param string $header
     * @return Vector<Titon\Http\AcceptHeader>
     */
    protected function _extractAcceptHeaders(string $header): Vector<AcceptHeader> {
        $data = Vector {};

        if ($accepts = $this->headers->get($header)) {
            invariant($accepts instanceof Traversable, 'Accepts header must be traversable.');

            foreach ($accepts as $accept) {
                foreach (explode(',', $accept) as $type) {
                    $type = trim($type);
                    $quality = 1;

                    if (strpos($type, ';') !== false) {
                        list($type, $quality) = explode(';', $type);
                    }

                    $data[] = shape(
                        'value' => strtolower($type),
                        'quality' => (float) str_replace('q=', '', $quality)
                    );
                }
            }
        }

        return $data;
    }

}

