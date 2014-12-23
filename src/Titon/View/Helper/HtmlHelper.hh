<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Utility\Config;
use Titon\Utility\Crypt;

/**
 * The HtmlHelper is primarily used for dynamic HTML tag creation within templates.
 *
 * @package Titon\View\Helper
 */
class HtmlHelper extends AbstractHelper {

    /**
     * {@inheritdoc}
     */
    protected TagMap $_tags = Map {
        'anchor'    => '<a{attr}>{body}</a>',
        'link'      => '<link{attr}>',
        'meta'      => '<meta{attr}>',
        'script'    => '<script{attr}>{body}</script>',
        'style'     => '<style{attr}>{body}</style>',
        'image'     => '<img{attr}>'
    };

    /**
     * Create an HTML anchor link.
     *
     * @param string $title
     * @param string $url
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function anchor(string $title, string $url, AttributeMap $attributes = Map {}): string {
        $attributes['href'] = $url;

        return $this->tag('anchor', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title)
        });
    }

    /**
     * Return the HTML5 doctype.
     *
     * @return string
     */
    public function doctype(): string {
        return '<!DOCTYPE html>' . PHP_EOL;
    }

    /**
     * Create an image element.
     *
     * @param string $path
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @param string $url
     * @return string
     */
    public function image(string $path, AttributeMap $attributes = Map {}, string $url = ''): string {
        if (!$attributes->contains('alt')) {
            $attributes['alt'] = '';
        }

        $attributes['src'] = $path;

        $image = $this->tag('image', Map {
            'attr' => $this->attributes($attributes)
        });

        if ($url) {
            return $this->tag('anchor', Map {
                'attr' => $this->attributes(Map {'href' => $url}),
                'body' => trim($image)
            });
        }

        return $image;
    }

    /**
     * Create a link element.
     *
     * @param string $path
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function link(string $path, AttributeMap $attributes = Map {}): string {
        $attributes = (Map {
            'rel'   => 'stylesheet',
            'type'  => 'text/css',
            'media' => 'screen'
        })->setAll($attributes);

        $attributes['href'] = $path;

        return $this->tag('link', Map {
            'attr' => $this->attributes($attributes)
        });
    }

    /**
     * Creates a mailto hyperlink. Emails will be obfuscated to hide against spambots and harvesters.
     *
     * @param string $email
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function mailto(string $email, AttributeMap $attributes = Map {}): string {
        $email = Crypt::obfuscate($email);

        if (!$attributes->contains('title')) {
            $attributes['title'] = '';
        }

        $attributes['escape'] = ['href'];
        $attributes['href'] = 'mailto:' . $email;

        return $this->tag('anchor', Map {
            'attr' => $this->attributes($attributes),
            'body' => $email
        });
    }

    /**
     * Create a meta element. Has predefined values for common meta tags.
     *
     * @param string|Map $type
     * @param string $content
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return string
     */
    public function meta(mixed $type, string $content = '', AttributeMap $attributes = Map {}): string {
        if ($type instanceof Map) {
            return $this->tag('meta', Map {
                'attr' => $this->attributes($type)
            });
        } else {
            $type = strtolower((string) $type);
        }

        if (!$content) {
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

        $metaTypes = Map {
            'content-type'          => Map {'http-equiv' => 'Content-Type', 'content' => $content},
            'content-script-type'   => Map {'http-equiv' => 'Content-Script-Type', 'content' => $content},
            'content-style-type'    => Map {'http-equiv' => 'Content-Style-Type', 'content' => $content},
            'content-language'      => Map {'http-equiv' => 'Content-Language', 'content' => $content},
            'keywords'              => Map {'name' => 'keywords', 'content' => $content},
            'description'           => Map {'name' => 'description', 'content' => $content},
            'author'                => Map {'name' => 'author', 'content' => $content},
            'robots'                => Map {'name' => 'robots', 'content' => $content},
            'rss'                   => Map {'type' => 'application/rss+xml', 'rel' => 'alternate', 'title' => '', 'link' => $content},
            'atom'                  => Map {'type' => 'application/atom+xml', 'title' => '', 'link' => $content},
            'icon'                  => Map {'type' => 'image/x-icon', 'rel' => 'icon', 'link' => $content},
        };

        if ($metaTypes->contains($type)) {
            $attributes = $metaTypes[$type]->setAll($attributes);
        } else {
            $attributes['name'] = $type;
            $attributes['content'] = $content;
        }

        return $this->tag('meta', Map {
            'attr' => $this->attributes($attributes)
        });
    }

    /**
     * Create a script element to include a JS file or to encompass JS code.
     *
     * @param string $source
     * @param bool $isBlock
     * @return string
     */
    public function script(string $source, bool $isBlock = false): string {
        $attributes = Map {'type' => 'text/javascript'};
        $content = '';

        if ($isBlock) {
            $content = '<![CDATA[' . $source . ']]>';
        } else {
            $attributes['src'] = $source;
        }

        return $this->tag('script', Map {
            'attr' => $this->attributes($attributes),
            'body' => $content
        });
    }

    /**
     * Create a style element to encompass CSS.
     *
     * @param string $content
     * @return string
     */
    public function style(string $content): string {
        return $this->tag('style', Map {
            'attr' => $this->attributes(Map {'type' => 'text/css'}),
            'body' => $content
        });
    }

    /**
     * Grab the page title if it has been set.
     *
     * @param string $separator
     * @return string
     */
    public function title(string $separator = ' - '): string {
        $view = $this->getView();

        if (!$view) {
            return '';
        }

        $pageTitle = $view->getVariable('pageTitle');

        if ($pageTitle instanceof Traversable) {
            return implode($separator, $pageTitle);
        }

        return (string) $pageTitle;
    }

}