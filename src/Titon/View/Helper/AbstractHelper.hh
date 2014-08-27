<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Psr\Http\Message\RequestInterface;
use Titon\Common\Base;
use Titon\Common\Attachable;
use Titon\Event\Event;
use Titon\Utility\Sanitize;
use Titon\Utility\Str;
use Titon\View\Helper;
use Titon\View\View;
use \Traversable;

/**
 * The Helper class acts as the base for all children helpers to extend.
 * Defines methods and properties for HTML tags and attribute generation.
 *
 * @package Titon\View\Helper
 */
abstract class AbstractHelper extends Base implements Helper {
    use Attachable;

    /**
     * Configuration.
     *
     * @type Map<string, mixed> {
     *      @type bool $escape  Global escaping
     * }
     */
    protected Map<string, mixed> $_config = Map {
        'escape' => true
    };

    /**
     * Mapping of HTML tags.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_tags = Map {};

    /**
     * Request object.
     *
     * @type \Psr\Http\Message\RequestInterface
     */
    protected ?RequestInterface $_request;

    /**
     * View object.
     *
     * @type \Titon\View\View
     */
    protected ?View $_view;

    /**
     * Parses an array of attributes to the HTML equivalent.
     *
     * @param Map<string, mixed> $attributes
     * @param Vector<string> $remove
     * @return string
     */
    public function attributes(Map<string, mixed> $attributes, Vector<string> $remove = Vector {}): string {
        $parsed = '';
        $escape = true;

        if (isset($attributes['escape'])) {
            $escape = $attributes['escape'];
            $remove[] = 'escape';
        }

        if ($attributes) {
            ksort($attributes);

            foreach ($attributes as $key => $value) {
                if (in_array($key, $remove)) {
                    continue;
                }

                if (($escape instanceof Traversable && !in_array($key, $escape)) || ($escape === true)) {
                    $value = $this->escape((string) $value, true);
                }

                $parsed .= ' ' . strtolower($key) . '="' . $value . '"';
            }
        }

        return $parsed;
    }

    /**
     * Escape a value.
     *
     * @param string $value
     * @param bool|null $escape
     * @return string
     */
    public function escape(string $value, ?bool $escape = null): string {
        if ($escape === null) {
            $escape = $this->getConfig('escape');
        }

        if ($escape) {
            $value = Sanitize::escape($value);
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function getRequest(): ?RequestInterface {
        return $this->_request;
    }

    /**
     * {@inheritdoc}
     */
    public function getView(): ?View {
        return $this->_view;
    }

    /**
     * {@inheritdoc}
     */
    public function preRender(Event $event, View $view, mixed &$template): void {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function postRender(Event $event, View $view, string &$response): void {
        $this->setView($view);
    }

    /**
     * {@inheritdoc}
     */
    public function registerEvents(): Map<string, mixed> {
        return Map {
            'view.preRender' => 'preRender',
            'view.postRender' => 'postRender',
        };
    }

    /**
     * {@inheritdoc}
     */
    public function setRequest(RequestInterface $request): this {
        $this->_request = $request;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view): this {
        $this->_view = $view;

        return $this;
    }

    /**
     * Generates an HTML tag if it exists.
     *
     * @param string $tag
     * @param Map<string, mixed> $params
     * @return string
     */
    public function tag(string $tag, Map<string, mixed> $params = Map {}) {
        return Str::insert($this->_tags[$tag], $params, Map {'escape' => false}) . PHP_EOL;
    }

}