<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server;

use Psr\Http\Message\RequestInterface;
use Titon\Common\FactoryAware;
use Titon\Http\AbstractMessage;
use Titon\Http\Bag\CookieBag;
use Titon\Http\Bag\FileBag;
use Titon\Http\Bag\ParameterBag;
use Titon\Http\Exception\InvalidMethodException;
use Titon\Http\Http;
use Titon\Http\Mime;
use Titon\Utility\Hash;

/**
 * The Request object is the primary source of data and state management for the environment.
 * It extracts and cleans the GET, POST and FILES data from the current HTTP request.
 *
 * @package Titon\Http\Server
 */
class Request extends AbstractMessage implements RequestInterface {
    use FactoryAware;

    /**
     * COOKIE data for the request.
     *
     * @type \Titon\Http\Bag\CookieBag
     */
    public $cookies;

    /**
     * An combined array of GET, POST, and FILES data.
     *
     * @type array
     */
    public $data = [];

    /**
     * FILES data for the request.
     *
     * @type \Titon\Http\Bag\FileBag
     */
    public $files;

    /**
     * GET data for the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public $get;

    /**
     * Data that has been generated internally via the framework during the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public $internal;

    /**
     * POST data for the request.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public $post;

    /**
     * List of server and environment variables.
     *
     * @type \Titon\Http\Bag\ParameterBag
     */
    public $server;

    /**
     * Configuration.
     *
     * @type array {
     *      @type bool $trustProxies    When enabled, will use applicable HTTP headers set by proxies
     * }
     */
    protected $_config = [
        'trustProxies' => true
    ];

    /**
     * The current type of request method.
     *
     * @type string
     */
    protected $_method;

    /**
     * Session instance.
     *
     * @type \Titon\Http\Server\Session
     */
    protected $_session;

    /**
     * The current URL for the request.
     *
     * @type string
     */
    protected $_url;

    /**
     * Load post data, query data, files data, cookies, server and environment settings.
     *
     * @param array $query
     * @param array $post
     * @param array $files
     * @param array $cookies
     * @param array $server
     */
    public function __construct(array $query = [], array $post = [], array $files = [], array $cookies = [], array $server = []) {
        parent::__construct();

        $this->internal = (new ParameterBag())->setRequest($this);
        $this->get = (new ParameterBag($query))->setRequest($this);
        $this->post = (new ParameterBag($post))->setRequest($this);
        $this->files = (new FileBag($files))->setRequest($this);
        $this->cookies = (new CookieBag($cookies))->setRequest($this);
        $this->server = (new ParameterBag($server))->setRequest($this);
        $this->data = Hash::merge($this->get->all(), $this->post->all(), $this->files->all());

        // Extract headers from server
        $headers = [];

        foreach ($server as $key => $value) {
            if (substr($key, 0, 5) === 'HTTP_') {
                $key = substr($key, 5);

            } else if (!in_array($key, ['CONTENT_LENGTH', 'CONTENT_MD5', 'CONTENT_TYPE'])) {
                continue;
            }

            $headers[$key] = explode(',', $value);
        }

        $this->headers->setRequest($this);
        $this->setHeaders($headers);
    }

    /**
     * Clone the bags.
     */
    public function __clone() {
        $this->internal = clone $this->internal;
        $this->headers = clone $this->headers;
        $this->get = clone $this->get;
        $this->post = clone $this->post;
        $this->files = clone $this->files;
        $this->cookies = clone $this->cookies;
        $this->server = clone $this->server;
    }

    /**
     * Create a request object based off the super globals.
     *
     * @return $this
     */
    public static function createFromGlobals() {
        if (isset($_POST['_method'])) {
            $_SERVER['REQUEST_METHOD'] = $_POST['_method'];
            unset($_POST['_method']);
        }

        return new static($_GET, $_POST, $_FILES, $_COOKIE, $_SERVER);
    }

    /**
     * Checks to see if the client accepts a certain content type, based on the Accept header.
     *
     * @uses Titon\Http\Mime
     *
     * @param string $type
     * @return array
     */
    public function accepts($type) {
        if (is_array($type)) {
            $contentType = $type;
        } else if (strpos($type, '/') !== false) {
            $contentType = [$type];
        } else {
            $contentType = (array) Mime::getTypeByExt($type);
        }

        foreach ($this->_accepts('Accept') as $accept) {
            foreach ($contentType as $cType) {
                if ($cType === $accept['type'] || $accept['type'] === '*/*') {
                    return $accept;

                // Wildcard matching
                } else if (strpos($accept['type'], '/*') && strpos($cType, trim($accept['type'], '*')) === 0) {
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
     * @return array
     */
    public function acceptsCharset($charset) {
        foreach ($this->_accepts('Accept-Charset') as $accept) {
            if (strtolower($charset) === $accept['type'] || $accept['type'] === '*') {
                return $accept;
            }
        }

        return null;
    }

    /**
     * Checks to see if the client accepts a certain encoding, based on the Accept-Encoding header.
     *
     * @param string $encoding
     * @return array
     */
    public function acceptsEncoding($encoding) {
        foreach ($this->_accepts('Accept-Encoding') as $accept) {
            if (strtolower($encoding) === $accept['type'] || $accept['type'] === '*') {
                return $accept;
            }
        }

        return null;
    }

    /**
     * Checks to see if the client accepts a certain charset, based on the Accept-Language header.
     *
     * @param string $language
     * @return array
     */
    public function acceptsLanguage($language) {
        foreach ($this->_accepts('Accept-Language') as $accept) {
            if (strtolower($language) === $accept['type'] || $accept['type'] === '*') {
                return $accept;
            }
        }

        return null;
    }

    /**
     * Get the IP address of the client, the correct way.
     * If trust proxies is enabled, use appropriate headers.
     *
     * @return string
     */
    public function getClientIP() {
        $headers = ['REMOTE_ADDR', 'HTTP_CLIENT_IP'];
        $ip = null;

        if ($this->getConfig('trustProxies')) {
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

        return $ip;
    }

    /**
     * Return a cookie by key and decrypt if encryption is enabled.
     *
     * @param string $key
     * @return mixed
     */
    public function getCookie($key) {
        return $this->cookies->get($key);
    }

    /**
     * Return all cookies and decrypt if encryption is enabled.
     *
     * @return array
     */
    public function getCookies() {
        $cookies = [];

        foreach ($this->cookies->keys() as $key) {
            $cookies[$key] = $this->cookies->get($key);
        }

        return $cookies;
    }

    /**
     * {@inheritdoc}
     */
    public function getHost() {
        $headers = ['HTTP_HOST', 'SERVER_NAME', 'SERVER_ADDR'];
        $host = null;

        if ($this->getConfig('trustProxies')) {
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
    public function getMethod() {
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
    public function getProtocolVersion() {
        return str_replace('HTTP/', '', $this->server->get('SERVER_PROTOCOL', Http::HTTP_11));
    }

    /**
     * Return the valid port based on possible server environment variables and HTTP headers.
     * If trust proxies is enabled, use appropriate headers.
     *
     * @return int
     */
    public function getPort() {
        if ($this->getConfig('trustProxies')) {
            if ($port = $this->server->get('HTTP_X_FORWARDED_PORT')) {
                return $port;
            }

            if ($this->server->get('HTTP_X_FORWARDED_PROTO') === 'https') {
                return 443;
            }
        }

        if ($host = $this->server->get('HTTP_HOST')) {
            if (strpos($host, ':') !== false) {
                return explode(':', $host)[1];
            }

            return ($this->getScheme() === 'https') ? 443 : 80;
        }

        return $this->server->get('SERVER_PORT');
    }

    /**
     * Get the referring URL. Will strip the hostname if it comes from the same domain.
     *
     * @return string
     */
    public function getReferrer() {
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
    public function getScheme() {
        return $this->isSecure() ? 'https' : 'http';
    }

    /**
     * Get the IP address of the current server.
     *
     * @return string
     */
    public function getServerIP() {
        return $this->server->get('SERVER_ADDR');
    }

    /**
     * Return the session manager.
     *
     * @return \Titon\Http\Server\Session
     */
    public function getSession() {
        return $this->_session;
    }

    /**
     * {@inheritdoc}
     */
    public function getUrl() {
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
    public function getUserAgent($explicit = false) {
        $agent = $this->server->get('HTTP_USER_AGENT');

        // @codeCoverageIgnoreStart
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
        // @codeCoverageIgnoreEnd

        return $agent;
    }

    /**
     * Returns true if the page was requested with AJAX.
     *
     * @return bool
     */
    public function isAJAX() {
        return (strtolower($this->server->get('HTTP_X_REQUESTED_WITH')) === 'xmlhttprequest');
    }

    /**
     * Returns true if the interface environment is CGI.
     *
     * @return bool
     * @codeCoverageIgnore
     */
    public function isCGI() {
        return (substr(PHP_SAPI, 0, 3) === 'cgi');
    }

    /**
     * Returns true if the interface environment is CLI (command line).
     *
     * @return bool
     * @codeCoverageIgnore
     */
    public function isCLI() {
        return (substr(PHP_SAPI, 0, 3) === 'cli');
    }

    /**
     * Returns true if the page was requested with a DELETE method.
     *
     * @return bool
     */
    public function isDelete() {
        return $this->isMethod('delete');
    }

    /**
     * Returns true if the page was requested through Flash.
     *
     * @return bool
     */
    public function isFlash() {
        return (bool) preg_match('/^(shockwave|adobe) flash/i', $this->getUserAgent(false));
    }

    /**
     * Returns true if the page was requested with a GET method.
     *
     * @return bool
     */
    public function isGet() {
        return $this->isMethod('get');
    }

    /**
     * Returns true if the interface environment is IIS.
     *
     * @return bool
     * @codeCoverageIgnore
     */
    public function isIIS() {
        return (substr(PHP_SAPI, 0, 5) === 'isapi');
    }

    /**
     * Primary container function for all method type checking. Returns true if the current request method matches the given argument.
     *
     * @param string $type
     * @return bool
     */
    public function isMethod($type) {
        return (strtoupper($type) === $this->getMethod());
    }

    /**
     * Returns true if the page was requested with a mobile device.
     *
     * @link http://detectmobilebrowsers.com/
     *
     * @return bool
     */
    public function isMobile() {
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
    public function isPost() {
        return $this->isMethod('post');
    }

    /**
     * Returns true if the page was requested with a PUT method.
     *
     * @return bool
     */
    public function isPut() {
        return $this->isMethod('put');
    }

    /**
     * Is the page being requested through a secure connection.
     *
     * @return bool
     */
    public function isSecure() {
        if ($this->getConfig('trustProxies')) {
            if ($scheme = $this->server->get('HTTP_X_FORWARDED_PROTO')) {
                return ($scheme === 'https');
            }
        }

        return (strtolower($this->server->get('HTTPS')) === 'on' || $this->server->get('SERVER_PORT') == 443);
    }

    /**
     * {@inheritdoc}
     */
    public function setMethod($method) {
        $method = strtoupper($method);

        if (!in_array($method, Http::getMethodTypes())) {
            throw new InvalidMethodException(sprintf('Invalid method %s', $method));
        }

        $this->_method = $method;
        $this->server->set('REQUEST_METHOD', $method);

        return $this;
    }

    /**
     * Set the session manager.
     *
     * @param \Titon\Http\Server\Session $session
     * @return $this
     */
    public function setSession(Session $session) {
        $this->_session = $session;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setUrl($url) {
        $this->_url = $url;

        return $this;
    }

    /**
     * Lazy loading functionality for extracting Accept header information and parsing it.
     *
     * @param string $header
     * @return array
     */
    protected function _accepts($header) {
        $data = [];

        if ($accept = $this->headers->get($header)) {
            foreach (explode(',', $accept) as $type) {
                $type = str_replace(' ', '', $type);

                if (strpos($type, ';') !== false) {
                    list($type, $quality) = explode(';', $type);
                } else {
                    $quality = 1;
                }

                $data[] = [
                    'type' => strtolower($type),
                    'quality' => str_replace('q=', '', $quality)
                ];
            }
        }

        return $data;
    }

}

