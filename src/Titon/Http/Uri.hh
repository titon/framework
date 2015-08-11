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
 * @todo
 *
 * @package Titon\Http
 */
class Uri implements UriInterface {

    /** @var string */
    protected string $fragment = '';

    /** @var string */
    protected string $host = '';

    /** @var string */
    protected string $password = '';

    /** @var string */
    protected string $path = '';

    /** @var int */
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

        if ($port = $this->getPort()) {
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
        if ($this->isStandardPort($this->port, $this->getScheme())) {
            return null;
        }

        return $this->port;
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
    public function isStandardPort(int $port, string $scheme) {
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
        if ($fragment === $this->getFragment()) {
            return $this;
        }

        $self = clone $this;
        $self->fragment = $self->filterFragment((string) $fragment);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withHost($host): this {
        if ($host === $this->getHost()) {
            return $this;
        }

        $self = clone $this;
        $self->host = $self->filterHost((string) $host);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withPath($path): this {
        if ($path === $this->getPath()) {
            return $this;
        }

        $self = clone $this;
        $self->path = $self->filterPath((string) $path);

        return $self; // TODO - error checking
    }

    /**
     * {@inheritdoc}
     */
    public function withPort($port): this {
        if ($port === $this->getPort()) {
            return $this;
        }

        $self = clone $this;
        $self->port = null;

        if ($port === null) {
            return $self;
        }

        $port = $self->filterPort((string) $port);

        if ($port < 1 || $port > 65535) {
            throw new InvalidArgumentException(sprintf('Invalid port [%d] specified; must fall within the valid TCP/UDP port range.', $port));
        }

        $self->port = $port;

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withQuery($query): this {
        if ($query === $this->getQuery()) {
            return $this;
        }

        $self = clone $this;
        $self->query = $self->filterQuery((string) $query);

        return $self; // TODO - error checking
    }

    /**
     * {@inheritdoc}
     */
    public function withScheme($scheme): this {
        if ($scheme === $this->getScheme()) {
            return $this;
        }

        $self = clone $this;
        $self->scheme = $self->filterScheme((string) $scheme);

        return $self;
    }

    /**
     * {@inheritdoc}
     */
    public function withUserInfo($user, $password = null): this {
        $self = clone $this;
        $self->user = $self->filterUser((string) $user);
        $self->password = '';

        if ($password) {
            $self->password = $self->filterPassword((string) $password);
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
     * @param string $fragment
     * @return string
     */
    protected function filterFragment(string $fragment): string {
        if ($fragment[0] === '#') {
            $fragment = substr($fragment, 1);
        }

        return $fragment;
    }

    /**
     * Filter the host name.
     *
     * @param string $host
     * @return string
     */
    protected function filterHost(string $host): string {
        return strtolower($host);
    }

    /**
     * Filter the user password.
     *
     * @param string $password
     * @return string
     */
    protected function filterPassword(string $password): string {
        return (string) $password;
    }

    /**
     * Filter the path. @todo
     *
     * @param string $path
     * @return string
     */
    protected function filterPath(string $path): string {
        return (string) $path; // TODO
    }

    /**
     * Filter the port and convert to an integer.
     *
     * @param string $port
     * @return string
     */
    protected function filterPort(string $port): int {
        return (int) $port;
    }

    /**
     * Filter the query. @todo
     *
     * @param string $query
     * @return string
     */
    protected function filterQuery(string $query): string {
        return (string) $query; // TODO
    }

    /**
     * Filter the scheme and remove any unnecessary scheme artifacts.
     *
     * @param string $scheme
     * @return string
     */
    protected function filterScheme(string $scheme): string {
        return strtolower(str_replace([':', '//'], '', $scheme));
    }

    /**
     * Filter the user name.
     *
     * @param string $user
     * @return string
     */
    protected function filterUser(string $user): string {
        return (string) $user;
    }
}
