<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\UriInterface;
use Titon\Http\Exception\MalformedUrlException;
use InvalidArgumentException;

/**
 * Implements a URI according to the PSR specification.
 *
 * @package Titon\Http
 */
class Uri implements UriInterface {

    /**
     * General delimiters.
     */
    const GEN_DELIMS = ':\/\?\#\[\]@';

    /**
     * Sub-component delimiters.
     */
    const SUB_DELIMS = '!\$&\'\(\)\*\+,;=';

    /**
     * Unreserved characters.
     */
    const UNRESERVED = 'a-zA-Z0-9\-\._~';

    /** @var string */
    protected string $fragment = '';

    /** @var string */
    protected string $host = '';

    /** @var string */
    protected string $password = '';

    /** @var string */
    protected string $path = '';

    /** @var int|null */
    protected ?int $port = null;

    /** @var string */
    protected string $query = '';

    /** @var string */
    protected string $scheme = '';

    /** @var string */
    protected string $user = '';

    /**
     * If a URI is passed, parse it into its components.
     *
     * @param string $uri
     */
    public function __construct(string $uri = '') {
        if ($uri) {
            $this->parse($uri);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function __toString(): string {
        return $this->format();
    }

    /**
     * Encode the URI based on RFC 3986, and attempt to avoid double encodings.
     *
     * @param string $uri
     * @return string
     */
    public function encode(string $uri): string {
        if (!$uri) {
            return $uri;
        }

        // If they match, the string was either decoded correctly, or not encoded before
        if ($uri === rawurldecode($uri)) {
            return rawurlencode($uri);
        }

        // We can assume that the URI has been encoded previously
        return $uri;
    }

    /**
     * Format the URI into a human readable form by combining all components.
     *
     * @return string
     */
    public function format(): string {
        $uri = '';

        if ($scheme = $this->getScheme()) {
            $uri .= sprintf('%s://', $scheme);
        }

        if ($authority = $this->getAuthority()) {
            $uri .= $authority;
        }

        if ($path = $this->getPath()) {
            if (!$path || $path[0] !== '/') {
                $path = '/' . $path;
            }

            $uri .= $path;
        }

        if ($query = $this->getQuery()) {
            $uri .= sprintf('?%s', $query);
        }

        if ($fragment = $this->getFragment()) {
            $uri .= sprintf('#%s', $fragment);
        }

        return $uri;
    }

    /**
     * {@inheritdoc}
     */
    public function getAuthority(): string {
        $authority = $this->getHost();

        if (!$authority) {
            return '';
        }

        if ($userInfo = $this->getUserInfo()) {
            $authority = $userInfo . '@' . $authority;
        }

        if (($port = $this->getPort()) !== null) {
            $authority .= ':' . $port;
        }

        return $authority;
    }

    /**
     * {@inheritdoc}
     */
    public function getFragment(): string {
        return $this->fragment;
    }

    /**
     * {@inheritdoc}
     */
    public function getHost(): string {
        return $this->host;
    }

    /**
     * {@inheritdoc}
     */
    public function getPath(): string {
        return $this->path;
    }

    /**
     * {@inheritdoc}
     */
    public function getPort(): ?int {
        return ($this->port === null || $this->isStandardPort($this->port, $this->getScheme())) ? null : $this->port;
    }

    /**
     * {@inheritdoc}
     */
    public function getQuery(): string {
        return $this->query;
    }

    /**
     * {@inheritdoc}
     */
    public function getScheme(): string {
        return $this->scheme;
    }

    /**
     * {@inheritdoc}
     */
    public function getUserInfo(): string {
        $user = $this->user;

        if ($password = $this->password) {
            $user .= ':' . $password;
        }

        return $user;
    }

    /**
     * Check whether the defined port and scheme is a standard HTTP or HTTPS port.
     *
     * @param int $port
     * @param string $scheme
     * @return bool
     */
    public function isStandardPort(int $port, string $scheme): bool {
        $standard = Map {
            'http' => 80,
            'https' => 443
        };

        return ($standard->containsKey($scheme) && $standard[$scheme] === $port);
    }

    /**
     * {@inheritdoc}
     */
    public function withFragment($fragment): this {
        $self = clone $this;
        $self->fragment = $self->filterFragment($fragment);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withHost($host): this {
        $self = clone $this;
        $self->host = $self->filterHost($host);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withPath($path): this {
        $self = clone $this;
        $self->path = $self->filterPath($path);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withPort($port): this {
        $self = clone $this;
        $self->port = null;

        if ($port === null) {
            return $self;
        }

        $self->port = $self->filterPort($port);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withQuery($query): this {
        $self = clone $this;
        $self->query = $self->filterQuery($query);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withScheme($scheme): this {
        $self = clone $this;
        $self->scheme = $self->filterScheme($scheme);
        $self->port = null;

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withUserInfo($user, $password = ''): this {
        $self = clone $this;
        $self->user = $self->filterUser($user);
        $self->password = '';

        if ($password) {
            $self->password = $self->filterPassword($password);
        }

        return $self;
    }

    /**
     * Parse the URI into all of its components. Each component will be passed through a secondary filter method.
     *
     * @param string $uri
     */
    protected function parse(string $uri): void {
        $parts = parse_url($uri);

        if ($parts === false) {
            throw new MalformedUrlException('Source URI appears to be malformed.');
        }

        $this->scheme   = array_key_exists('scheme', $parts) ? $this->filterScheme($parts['scheme']) : '';
        $this->host     = array_key_exists('host', $parts) ? $this->filterHost($parts['host']) : '';
        $this->port     = array_key_exists('port', $parts) ? $this->filterPort($parts['port']) : null;
        $this->user     = array_key_exists('user', $parts) ? $this->filterUser($parts['user']) : '';
        $this->password = array_key_exists('pass', $parts) ? $this->filterPassword($parts['pass']) : '';
        $this->path     = array_key_exists('path', $parts) ? $this->filterPath($parts['path']) : '';
        $this->query    = array_key_exists('query', $parts) ? $this->filterQuery($parts['query']) : '';
        $this->fragment = array_key_exists('fragment', $parts) ? $this->filterFragment($parts['fragment']) : '';
    }

    /**
     * Filter the fragment by removing the leading hash.
     *
     * @param mixed $fragment
     * @return string
     */
    protected function filterFragment(mixed $fragment): string {
        $fragment = trim((string) $fragment);

        if (!$fragment) {
            return '';
        }

        if ($fragment[0] === '#') {
            $fragment = substr($fragment, 1);
        }

        return $this->encode($fragment);
    }

    /**
     * Filter the host name.
     *
     * @param mixed $host
     * @return string
     * @throws \InvalidArgumentException
     */
    protected function filterHost(mixed $host): string {
        $host = strtolower(trim((string) $host));

        if (!$host) {
            return '';
        }

        if (!preg_match('/^[-a-z0-9\.]+$/', $host) || $host[0] === '-' || substr($host, -1) === '-') {
            throw new InvalidArgumentException(sprintf('Invalid host [%s] specified; may only contain letters, numbers, periods, and hyphens.', $host));
        }

        return $host;
    }

    /**
     * Filter the user password.
     *
     * @param mixed $password
     * @return string
     */
    protected function filterPassword(mixed $password): string {
        return $this->encode(trim((string) $password));
    }

    /**
     * Filter the path.
     *
     * @param mixed $path
     * @return string
     * @throws \InvalidArgumentException
     */
    protected function filterPath(mixed $path): string {
        $path = trim((string) $path);

        if (strpos($path, '?') !== false || strpos($path, '#') !== false) {
            throw new InvalidArgumentException(sprintf('Invalid path [%s] specified; must not contain a query string or fragment.', $path));
        }

        if ($path === '' || $path === '/') {
            return $path;
        }

        // Only encode each segment, not the separators
        return implode('/', array_map(inst_meth($this, 'encode'), explode('/', $path)));
    }

    /**
     * Filter the port and convert to an integer.
     *
     * @param mixed $port
     * @return int
     * @throws \InvalidArgumentException
     */
    protected function filterPort(mixed $port): int {
        $port = (int) $port;

        if ($port < 1 || $port > 65535) {
            throw new InvalidArgumentException(sprintf('Invalid port [%d] specified; must fall within the valid TCP/UDP port range.', $port));
        }

        return $port;
    }

    /**
     * Filter the query.
     *
     * @param mixed $query
     * @return string
     */
    protected function filterQuery(mixed $query): string {
        $query = trim((string) $query);

        if (!$query) {
            return '';
        }

        if ($query[0] === '?') {
            $query = substr($query, 1);
        }

        $parts = [];

        foreach (explode('&', $query) as $part) {
            $data = explode('=', $part, 2);

            if (array_key_exists(1, $data)) {
                $parts[] = sprintf('%s=%s', $this->encode($data[0]), $this->encode($data[1]));
            } else {
                $parts[] = $this->encode($data[0]);
            }
        }

        return implode('&', $parts);
    }

    /**
     * Filter the scheme and remove any unnecessary scheme artifacts.
     *
     * @param mixed $scheme
     * @return string
     * @throws \InvalidArgumentException
     */
    protected function filterScheme(mixed $scheme): string {
        $scheme = strtolower(preg_replace('/:(\/\/)?$/', '', trim((string) $scheme)));

        if (!$scheme) {
            return '';
        }

        if (!preg_match('/^[a-z]{1}[-a-z0-9\.\+]+$/', $scheme)) {
            throw new InvalidArgumentException(sprintf('Invalid scheme [%s] specified; may only contain letters, numbers, periods, hyphens, and plus signs.', $scheme));
        }

        return $scheme;
    }

    /**
     * Filter the user name.
     *
     * @param mixed $user
     * @return string
     */
    protected function filterUser(mixed $user): string {
        return $this->encode(trim((string) $user));
    }
}
