<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Titon\Http\Exception\InvalidStatusException;

/**
 * HTTP related constants and static variables.
 *
 * @package Titon\Http
 */
class Http {

    const DATE_FORMAT = 'D, d M Y H:i:s T';
    const HTTP_10 = 'HTTP/1.0';
    const HTTP_11 = 'HTTP/1.1';

    const CONTINUE_REQUEST = 100;
    const SWITCHING_PROTOCOLS = 101;
    const PROCESSING = 102;
    const OK = 200;
    const CREATED = 201;
    const ACCEPTED = 202;
    const NON_AUTHORITATIVE_INFORMATION = 203;
    const NO_CONTENT = 204;
    const RESET_CONTENT = 205;
    const PARTIAL_CONTENT = 206;
    const MULTI_STATUS = 207;
    const MULTIPLE_CHOICES = 300;
    const MOVED_PERMANENTLY = 301;
    const FOUND = 302;
    const SEE_OTHER = 303;
    const NOT_MODIFIED = 304;
    const USE_PROXY = 305;
    const TEMPORARY_REDIRECT = 307;
    const BAD_REQUEST = 400;
    const UNAUTHORIZED = 401;
    const PAYMENT_REQUIRED = 402;
    const FORBIDDEN = 403;
    const NOT_FOUND = 404;
    const METHOD_NOT_ALLOWED = 405;
    const NOT_ACCEPTABLE = 406;
    const PROXY_AUTHENTICATION_REQUIRED = 407;
    const REQUEST_TIMEOUT = 408;
    const CONFLICT = 409;
    const GONE = 410;
    const LENGTH_REQUIRED = 411;
    const PRECONDITION_FAILED = 412;
    const REQUEST_ENTITY_TOO_LARGE = 413;
    const REQUEST_URI_TOO_LONG = 414;
    const UNSUPPORTED_MEDIA_TYPE = 415;
    const REQUESTED_RANGE_NOT_SATISFIABLE = 416;
    const EXPECTATION_FAILED = 417;
    const UNPROCESSABLE_ENTITY = 422;
    const LOCKED = 423;
    const FAILED_DEPENDENCY = 424;
    const PRECONDITION_REQUIRED = 428;
    const TOO_MANY_REQUESTS = 429;
    const REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
    const INTERNAL_SERVER_ERROR = 500;
    const NOT_IMPLEMENTED = 501;
    const BAD_GATEWAY = 502;
    const SERVICE_UNAVAILABLE = 503;
    const GATEWAY_TIMEOUT = 504;
    const HTTP_VERSION_NOT_SUPPORTED = 505;
    const INSUFFICIENT_STORAGE = 507;
    const LOOP_DETECTED = 508;
    const NOT_EXTENDED = 510;
    const NETWORK_AUTHENTICATION_REQUIRED = 511;

    /**
     * List of acceptable header types.
     *
     * @type string[]
     */
    protected static $_headerTypes = [
        'Accept',
        'Accept-Charset',
        'Accept-Encoding',
        'Accept-Language',
        'Accept-Ranges',
        'Access-Control-Allow-Origin',
        'Age',
        'Allow',
        'Authentication-Info',
        'Authorization',
        'Cache-Control',
        'Connection',
        'Content-Disposition',
        'Content-Encoding',
        'Content-Language',
        'Content-Length',
        'Content-Location',
        'Content-MD5',
        'Content-Range',
        'Content-Type',
        'Cookie',
        'Date',
        'ETag',
        'Expires',
        'Expect',
        'From',
        'Host',
        'If-Match',
        'If-Modified-Since',
        'If-None-Match',
        'If-Range',
        'If-Unmodified-Since',
        'Keep-Alive',
        'Last-Modified',
        'Link',
        'Location',
        'Max-Forwards',
        'Origin',
        'P3P',
        'Pragma',
        'Proxy-Authenticate',
        'Proxy-Authorization',
        'Range',
        'Referer',
        'Refresh',
        'Retry-After',
        'Server',
        'Set-Cookie',
        'Status',
        'Strict-Transport-Security',
        'TE',
        'Trailer',
        'Transfer-Encoding',
        'Upgrade',
        'User-Agent',
        'Vary',
        'Via',
        'Warning',
        'WWW-Authenticate'
    ];

    /**
     * List of possible method types.
     *
     * @type string[]
     */
    protected static $_methodTypes = [
        'GET',
        'POST',
        'PUT',
        'DELETE',
        'HEAD',
        'TRACE',
        'OPTIONS',
        'CONNECT'
    ];

    /**
     * List of all available response status codes.
     *
     * @type string[]
     */
    protected static $_statusCodes = [
        100 => 'Continue',
        101 => 'Switching Protocols',
        102 => 'Processing', // RFC2518
        200 => 'OK',
        201 => 'Created',
        202 => 'Accepted',
        203 => 'Non-Authoritative Information',
        204 => 'No Content',
        205 => 'Reset Content',
        206 => 'Partial Content',
        207 => 'Multi-Status', // RFC4918
        300 => 'Multiple Choices',
        301 => 'Moved Permanently',
        302 => 'Found',
        303 => 'See Other',
        304 => 'Not Modified',
        305 => 'Use Proxy',
        307 => 'Temporary Redirect',
        400 => 'Bad Request',
        401 => 'Unauthorized',
        402 => 'Payment Required',
        403 => 'Forbidden',
        404 => 'Not Found',
        405 => 'Method Not Allowed',
        406 => 'Not Acceptable',
        407 => 'Proxy Authentication Required',
        408 => 'Request Timeout',
        409 => 'Conflict',
        410 => 'Gone',
        411 => 'Length Required',
        412 => 'Precondition Failed',
        413 => 'Request Entity Too Large',
        414 => 'Request-URI Too Long',
        415 => 'Unsupported Media Type',
        416 => 'Requested Range Not Satisfiable',
        417 => 'Expectation Failed',
        422 => 'Unprocessable Entity', // RFC4918
        423 => 'Locked', // RFC4918
        424 => 'Failed Dependency', // RFC4918
        428 => 'Precondition Required', // RFC6585
        429 => 'Too Many Requests', // RFC6585
        431 => 'Request Header Fields Too Large', // RFC6585
        500 => 'Internal Server Error',
        501 => 'Not Implemented',
        502 => 'Bad Gateway',
        503 => 'Service Unavailable',
        504 => 'Gateway Timeout',
        505 => 'HTTP Version Not Supported',
        507 => 'Insufficient Storage', // RFC4918
        508 => 'Loop Detected', // RFC5842
        510 => 'Not Extended', // RFC2774
        511 => 'Network Authentication Required', // RFC6585
    ];

    /**
     * Return all the standard types of HTTP headers.
     *
     * @return string[]
     */
    public static function getHeaderTypes() {
        return static::$_headerTypes;
    }

    /**
     * Return all the supported method types.
     *
     * @return string[]
     */
    public static function getMethodTypes() {
        return static::$_methodTypes;
    }

    /**
     * Get a single status code.
     *
     * @param int $code
     * @return string
     * @throws \Titon\Http\Exception\InvalidStatusException
     */
    public static function getStatusCode($code) {
        if (isset(static::$_statusCodes[$code])) {
            return static::$_statusCodes[$code];
        }

        throw new InvalidStatusException(sprintf('Status code %s is not supported', $code));
    }

    /**
     * Get all status codes.
     *
     * @return string[]
     */
    public static function getStatusCodes() {
        return static::$_statusCodes;
    }

}