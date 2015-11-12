<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * HTTP related constants and static variables.
 *
 * @package Titon\Http
 */
class Http {

    const string DATE_FORMAT = 'D, d M Y H:i:s T';
    const string HTTP_10 = 'HTTP/1.0';
    const string HTTP_11 = 'HTTP/1.1';

    /**
     * List of acceptable header types.
     *
     * @var \Titon\Http\HeaderList
     */
    protected static HeaderList $headerTypes = Set {
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
    };

    /**
     * List of possible method types.
     *
     * @var \Titon\Http\MethodList
     */
    protected static MethodList $methodTypes = Set {
        'GET',
        'POST',
        'PUT',
        'DELETE',
        'HEAD',
        'TRACE',
        'OPTIONS',
        'CONNECT',
        'PATCH'
    };

    /**
     * Return all the standard types of HTTP headers.
     *
     * @return \Titon\Http\HeaderList
     */
    public static function getHeaderTypes(): HeaderList {
        return static::$headerTypes;
    }

    /**
     * Return all the supported method types.
     *
     * @return \Titon\Http\MethodList
     */
    public static function getMethodTypes(): MethodList {
        return static::$methodTypes;
    }

}
