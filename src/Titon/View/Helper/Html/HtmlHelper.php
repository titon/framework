<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Html;

use Titon\View\Helper\AbstractHelper;
use Titon\Utility\Config;
use Titon\Utility\Crypt;

/**
 * The HtmlHelper is primarily used for dynamic HTML tag creation within templates.
 *
 * @package Titon\View\Helper\Html
 */
class HtmlHelper extends AbstractHelper {

    /**
     * Mapping of HTML tags for this helper.
     *
     * @type array
     */
    protected $_tags = [
        'anchor'    => '<a{attr}>{body}</a>',
        'link'      => '<link{attr}>',
        'meta'      => '<meta{attr}>',
        'script'    => '<script{attr}>{body}</script>',
        'style'     => '<style{attr}>{body}</style>',
        'image'     => '<img{attr}>'
    ];

    /**
     * Create an HTML anchor link.
     *
     * @param string $title
     * @param string $url
     * @param array $attributes
     * @return string
     */
    public function anchor($title, $url, array $attributes = []) {
        $attributes['href'] = (string) $url;

        return $this->tag('anchor', [
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title)
        ]);
    }

    /**
     * Return the HTML5 doctype.
     *
     * @return string
     */
    public function doctype() {
        return '<!DOCTYPE html>' . PHP_EOL;
    }

    /**
     * Create an image element.
     *
     * @param string $path
     * @param array $attributes
     * @param mixed $url
     * @return string
     */
    public function image($path, array $attributes = [], $url = null) {
        if (!isset($attributes['alt'])) {
            $attributes['alt'] = '';
        }

        $attributes['src'] = $path;

        $image = $this->tag('image', [
            'attr' => $this->attributes($attributes)
        ]);

        if ($url) {
            return $this->tag('anchor', [
                'attr' => $this->attributes(['href' => $url]),
                'body' => trim($image)
            ]);
        }

        return $image;
    }

    /**
     * Create a link element.
     *
     * @param string $path
     * @param array $attributes
     * @return string
     */
    public function link($path, array $attributes = []) {
        $attributes = $attributes + [
            'rel'   => 'stylesheet',
            'type'  => 'text/css',
            'media' => 'screen'
        ];

        $attributes['href'] = $path;

        return $this->tag('link', [
            'attr' => $this->attributes($attributes)
        ]);
    }

    /**
     * Creates a mailto hyperlink. Emails will be obfuscated to hide against spambots and harvesters.
     *
     * @param string $email
     * @param array $attributes
     * @return string
     */
    public function mailto($email, array $attributes = []) {
        $email = Crypt::obfuscate($email);

        if (!isset($attributes['title'])) {
            $attributes['title'] = '';
        }

        $attributes['escape'] = ['href'];
        $attributes['href'] = 'mailto:' . $email;

        return $this->tag('anchor', [
            'attr' => $this->attributes($attributes),
            'body' => $email
        ]);
    }

    /**
     * Create a meta element. Has predefined values for common meta tags.
     *
     * @param string $type
     * @param string $content
     * @param array $attributes
     * @return string
     */
    public function meta($type, $content = null, array $attributes = []) {
        if (is_array($type)) {
            return $this->tag('meta', [
                'attr' => $this->attributes($type)
            ]);
        } else {
            $type = mb_strtolower($type);
        }

        if (empty($content)) {
            switch ($type) {
                case 'content-script-type':
                    $content = 'text/javascript';
                break;
                case 'content-style-type':
                    $content = 'text/css';
                break;
                case 'content-type':
                    $content = 'text/html; charset=' . Config::encoding();
                break;
            }
        }

        $metaTypes = [
            'content-type'          => ['http-equiv' => 'Content-Type', 'content' => $content],
            'content-script-type'   => ['http-equiv' => 'Content-Script-Type', 'content' => $content],
            'content-style-type'    => ['http-equiv' => 'Content-Style-Type', 'content' => $content],
            'content-language'      => ['http-equiv' => 'Content-Language', 'content' => $content],
            'keywords'              => ['name' => 'keywords', 'content' => $content],
            'description'           => ['name' => 'description', 'content' => $content],
            'author'                => ['name' => 'author', 'content' => $content],
            'robots'                => ['name' => 'robots', 'content' => $content],
            'rss'                   => ['type' => 'application/rss+xml', 'rel' => 'alternate', 'title' => '', 'link' => $content],
            'atom'                  => ['type' => 'application/atom+xml', 'title' => '', 'link' => $content],
            'icon'                  => ['type' => 'image/x-icon', 'rel' => 'icon', 'link' => $content],
        ];

        if (isset($metaTypes[$type])) {
            $attributes = $attributes + $metaTypes[$type];
        } else {
            $attributes['name'] = $type;
            $attributes['content'] = $content;
        }

        return $this->tag('meta', [
            'attr' => $this->attributes($attributes)
        ]);
    }

    /**
     * Create a script element to include a JS file or to encompass JS code.
     *
     * @param string $source
     * @param bool $isBlock
     * @return string
     */
    public function script($source, $isBlock = false) {
        $attributes = ['type' => 'text/javascript'];
        $content = '';

        if ($isBlock) {
            $content = '<![CDATA[' . $source . ']]>';
        } else {
            $attributes['src'] = $source;
        }

        return $this->tag('script', [
            'attr' => $this->attributes($attributes),
            'body' => $content
        ]);
    }

    /**
     * Create a style element to encompass CSS.
     *
     * @param string $content
     * @return string
     */
    public function style($content) {
        return $this->tag('style', [
            'attr' => $this->attributes(['type' => 'text/css']),
            'body' => $content
        ]);
    }

    /**
     * Grab the page title if it has been set.
     *
     * @param string|array $separator
     * @return string
     */
    public function title($separator = ' - ') {
        $pageTitle = $this->getView()->getVariable('pageTitle');

        if (is_array($pageTitle)) {
            return implode($separator, $pageTitle);
        }

        return $pageTitle;
    }

}