<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Xhtml;

use Titon\View\Helper\Html\HtmlHelper;
use Titon\Utility\Sanitize;

/**
 * The XhtmlHelper is primarily used for dynamic HTML tag creation within templates.
 *
 * @package Titon\View\Helper\Xhtml
 */
class XhtmlHelper extends HtmlHelper {

    /**
     * Mapping of XHTML tags for this helper.
     *
     * @type array
     */
    protected $_tags = [
        'anchor'    => '<a{attr}>{body}</a>',
        'link'      => '<link{attr} />',
        'meta'      => '<meta{attr} />',
        'script'    => '<script{attr}>{body}</script>',
        'style'     => '<style{attr}>{body}</style>',
        'image'     => '<img{attr} />'
    ];

    /**
     * Return the XHTML doctype.
     *
     * @param string $type
     * @return string
     */
    public function doctype($type = 'default') {
        $type = mb_strtolower($type);

        if ($type === 'transitional') {
            $type = 'trans';
        }

        $docTypes = [
            'strict'    => '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">',
            'trans'     => '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">',
            'frameset'  => '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Frameset//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd">',
            'default'   => '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">'
        ];

        if (!isset($docTypes[$type])) {
            $type = 'default';
        }

        return $docTypes[$type] . PHP_EOL;
    }

    /**
     * Escape a value.
     *
     * @param string $value
     * @param bool|null $escape
     * @return string
     */
    public function escape($value, $escape = null) {
        if ($escape === null) {
            $escape = $this->getConfig('escape');
        }

        if ($escape) {
            $value = Sanitize::escape($value, [
                'flags' => ENT_QUOTES | ENT_XHTML
            ]);
        }

        return $value;
    }

}