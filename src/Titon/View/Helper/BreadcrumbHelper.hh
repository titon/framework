<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Utility\Traverse;
use Titon\Utility\Registry;

newtype breadcrumb = Map<string, mixed>;

/**
 * The BreadcrumbHelper is primarily used for adding and generating breadcrumb lists.
 *
 * @package Titon\View\Helper
 * @property \Titon\View\Helper\HtmlHelper $html
 */
class BreadcrumbHelper extends AbstractHelper {

    /**
     * A list of all breadcrumbs in the trail, with the title, url and attributes.
     *
     * @type Vector<breadcrumb>
     */
    protected Vector<breadcrumb> $_breadcrumbs = Vector {};

    /**
     * Add a link to the breadcrumbs.
     *
     * @param string|array $titles
     * @param string|array $url
     * @param Map<string, mixed> $attributes
     * @return $this
     */
    public function add(mixed $titles, mixed $url = '', Map<string, mixed> $attributes = Map {}): this {
        if (is_traversable($titles)) {
            foreach ($titles as $title => $url) {
                $this->append($title, $url, $attributes);
            }
        } else {
            $this->append($titles, $url, $attributes);
        }

        return $this;
    }

    /**
     * Return all breadcrumbs.
     *
     * @return Vector<breadcrumb>
     */
    public function all(): Vector<breadcrumb> {
        return $this->_breadcrumbs;
    }

    /**
     * Add a breadcrumb to the end of the list.
     *
     * @param string $title
     * @param string|array $url
     * @param Map<string, mixed> $attributes
     * @return $this
     */
    public function append(string $title, mixed $url, Map<string, mixed> $attributes = Map {}): this {
        $this->_breadcrumbs[] = Map {
            'title' => $title,
            'url' => $url,
            'attributes' => $attributes
        };

        return $this;
    }

    /**
     * Return the first crumb in the list.
     *
     * @return breadcrumb
     */
    public function first(): ?breadcrumb {
        return $this->_breadcrumbs->get(0);
    }

    /**
     * Return an array of breadcrumbs formatted as anchor links.
     *
     * @param Map<string, mixed> $attributes
     * @return Vector<string>
     */
    public function generate(Map<string, mixed> $attributes = Map {}): this {
        $trail = Vector {};

        if ($this->_breadcrumbs) {
            foreach ($this->_breadcrumbs as $crumb) {
                $trail[] = $this->html->anchor($crumb['title'], $crumb['url'], $crumb['attributes']->setAll($attributes));
            }
        }

        return $trail;
    }

    /**
     * Attach the HtmlHelper.
     */
    public function initialize(): void {
        $this->attachObject('html', function() {
            return Registry::factory('Titon\View\Helper\HtmlHelper');
        });
    }

    /**
     * Return the last crumb in the list.
     *
     * @return breadcrumb
     */
    public function last(): ?breadcrumb {
        return $this->_breadcrumbs->get(count($this->_breadcrumbs) - 1);
    }

    /**
     * Add a breadcrumb to the beginning of the list.
     *
     * @param string $title
     * @param string|array $url
     * @param Map<string, mixed> $attributes
     * @return $this
     */
    public function prepend(string $title, mixed $url, Map<string, mixed> $attributes = Map {}): this {
        array_unshift($this->_breadcrumbs, Map {
            'title' => $title,
            'url' => $url,
            'attributes' => $attributes
        });

        return $this;
    }

    /**
     * Generate a page title based off the current crumbs.
     *
     * @param string $base
     * @param Map<string, mixed> $options
     * @return string
     */
    public function title(string $base = '', Map<string, mixed> $options = Map {}): string {
        $options = Traverse::merge(Map {
            'reverse' => false,
            'depth' => 3,
            'separator' => ' - '
        }, $options);

        $crumbs = Traverse::pluck($this->_breadcrumbs, 'title')->toArray();
        $count = count($crumbs);
        $title = [];

        if ($count) {
            if ($options['depth'] && $count > $options['depth']) {
                $title = array_slice($crumbs, -$options['depth']);
                array_unshift($title, array_shift($crumbs));

            } else {
                $title = $crumbs;
            }

        } else if ($pageTitle = $this->html->title($options['separator'])) {
            $title[] = $pageTitle;
        }

        if ($options['reverse']) {
            $title = array_reverse($title);
        }

        if ($base) {
            array_unshift($title, $base);
        }

        return implode($options['separator'], array_unique($title));
    }

}