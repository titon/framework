<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Titon\Http\Exception\InvalidStatusException;

/**
 * Status code related constants and helper methods.
 *
 * @package Titon\Http
 */
class StatusCode {

    const int CONTINUE_REQUEST = 100;
    const int SWITCHING_PROTOCOLS = 101;
    const int PROCESSING = 102;
    const int OK = 200;
    const int CREATED = 201;
    const int ACCEPTED = 202;
    const int NON_AUTHORITATIVE_INFORMATION = 203;
    const int NO_CONTENT = 204;
    const int RESET_CONTENT = 205;
    const int PARTIAL_CONTENT = 206;
    const int MULTI_STATUS = 207;
    const int MULTIPLE_CHOICES = 300;
    const int MOVED_PERMANENTLY = 301;
    const int FOUND = 302;
    const int SEE_OTHER = 303;
    const int NOT_MODIFIED = 304;
    const int USE_PROXY = 305;
    const int TEMPORARY_REDIRECT = 307;
    const int BAD_REQUEST = 400;
    const int UNAUTHORIZED = 401;
    const int PAYMENT_REQUIRED = 402;
    const int FORBIDDEN = 403;
    const int NOT_FOUND = 404;
    const int METHOD_NOT_ALLOWED = 405;
    const int NOT_ACCEPTABLE = 406;
    const int PROXY_AUTHENTICATION_REQUIRED = 407;
    const int REQUEST_TIMEOUT = 408;
    const int CONFLICT = 409;
    const int GONE = 410;
    const int LENGTH_REQUIRED = 411;
    const int PRECONDITION_FAILED = 412;
    const int REQUEST_ENTITY_TOO_LARGE = 413;
    const int REQUEST_URI_TOO_LONG = 414;
    const int UNSUPPORTED_MEDIA_TYPE = 415;
    const int REQUESTED_RANGE_NOT_SATISFIABLE = 416;
    const int EXPECTATION_FAILED = 417;
    const int UNPROCESSABLE_ENTITY = 422;
    const int LOCKED = 423;
    const int FAILED_DEPENDENCY = 424;
    const int PRECONDITION_REQUIRED = 428;
    const int TOO_MANY_REQUESTS = 429;
    const int REQUEST_HEADER_FIELDS_TOO_LARGE = 431;
    const int INTERNAL_SERVER_ERROR = 500;
    const int NOT_IMPLEMENTED = 501;
    const int BAD_GATEWAY = 502;
    const int SERVICE_UNAVAILABLE = 503;
    const int GATEWAY_TIMEOUT = 504;
    const int HTTP_VERSION_NOT_SUPPORTED = 505;
    const int INSUFFICIENT_STORAGE = 507;
    const int LOOP_DETECTED = 508;
    const int NOT_EXTENDED = 510;
    const int NETWORK_AUTHENTICATION_REQUIRED = 511;

    /**
     * List of all available response status codes.
     *
     * @var \Titon\Http\StatusCodeMap
     */
    protected static StatusCodeMap $statusCodes = Map {
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
    };

    /**
     * Get a single status code.
     *
     * @param int $code
     * @return string
     * @throws \Titon\Http\Exception\InvalidStatusException
     */
    public static function getReasonPhrase(int $code): string {
        if (static::$statusCodes->contains($code)) {
            return static::$statusCodes[$code];
        }

        throw new InvalidStatusException(sprintf('Status code [%d] is not supported', $code));
    }

    /**
     * Get all status codes.
     *
     * @return \Titon\Http\StatusCodeMap
     */
    public static function getAll(): StatusCodeMap {
        return static::$statusCodes;
    }

    /**
     * Validate the status code is legitimate.
     *
     * @param int $code
     * @return int
     * @throws \Titon\Http\Exception\InvalidStatusException
     */
    public static function isValid(int $code): int {
        if (static::$statusCodes->contains($code)) {
            return $code;
        }

        throw new InvalidStatusException(sprintf('Status code [%d] is not supported', $code));
    }

}
