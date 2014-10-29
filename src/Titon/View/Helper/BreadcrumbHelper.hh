<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Common\OptionMap;
use Titon\Type\ArrayList;

type Breadcrumb = shape('title' => string, 'url' => string, 'attributes' => AttributeMap);

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
     * @type ArrayList<Breadcrumb>
     */
    protected ArrayList<Breadcrumb> $_breadcrumbs;

    /**
     * Initialize the ArrayList.
     */
    public function __construct() {
        $this->_breadcrumbs = new ArrayList();
    }

    /**
     * Add a link to the breadcrumbs.
     *
     * @param string|Traversable $titles
     * @param string $url
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return $this
     */
    public function add(mixed $titles, string $url = '', AttributeMap $attributes = Map {}): this {
        if ($titles instanceof Traversable) {
            foreach ($titles as $title => $url) {
                $this->append((string) $title, $url, $attributes);
            }
        } else {
            $this->append((string) $titles, $url, $attributes);
        }

        return $this;
    }

    /**
     * Add a breadcrumb to the end of the list.
     *
     * @param string $title
     * @param string $url
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return $this
     */
    public function append(string $title, string $url, AttributeMap $attributes = Map {}): this {
        $this->_breadcrumbs = $this->getBreadcrumbs()->append(shape(
            'title' => $title,
            'url' => $url,
            'attributes' => $attributes
        ));

        return $this;
    }

    /**
     * Return the first crumb in the list.
     *
     * @return \Titon\View\Helper\Breadcrumb
     */
    public function first(): ?Breadcrumb {
        return $this->getBreadcrumbs()->first();
    }

    /**
     * Return an array of breadcrumbs formatted as anchor links.
     *
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return Vector<string>
     */
    public function generate(AttributeMap $attributes = Map {}): Vector<string> {
        $trail = Vector {};

        /** @type \Titon\View\Helper\HtmlHelper $html */
        $html = $this->getHelper('html');

        foreach ($this->getBreadcrumbs() as $crumb) {
            $trail[] = $html->anchor($crumb['title'], $crumb['url'], $crumb['attributes']->setAll($attributes));
        }

        return $trail;
    }

    /**
     * Return all breadcrumbs.
     *
     * @return \Titon\Type\ArrayList<Breadcrumb>
     */
    public function getBreadcrumbs(): ArrayList<Breadcrumb> {
        return $this->_breadcrumbs;
    }

    /**
     * Return the last crumb in the list.
     *
     * @return \Titon\View\Helper\Breadcrumb
     */
    public function last(): ?Breadcrumb {
        return $this->getBreadcrumbs()->last();
    }

    /**
     * Add a breadcrumb to the beginning of the list.
     *
     * @param string $title
     * @param string $url
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @return $this
     */
    public function prepend(string $title, string $url, AttributeMap $attributes = Map {}): this {
        $this->_breadcrumbs = $this->getBreadcrumbs()->prepend(shape(
            'title' => $title,
            'url' => $url,
            'attributes' => $attributes
        ));

        return $this;
    }

    /**
     * Generate a page title based off the current crumbs.
     *
     * @param string $base
     * @param \Titon\Common\OptionMap $options
     * @return string
     */
    public function title(string $base = '', OptionMap $options = Map {}): string {
        $options = (Map {
            'reverse' => false,
            'depth' => 3,
            'separator' => ' - '
        })->setAll($options);

        $crumbs = $this->getBreadcrumbs()->pluck(($value, $key) ==> $value['title'])->toArray();
        $count = count($crumbs);
        $depth = (int) $options['depth'];
        $title = [];

        /** @type \Titon\View\Helper\HtmlHelper $html */
        $html = $this->getHelper('html');

        if ($count) {
            if ($depth && $count > $depth) {
                $title = array_slice($crumbs, -$depth);
                array_unshift($title, array_shift($crumbs));

            } else {
                $title = $crumbs;
            }

        } else if ($pageTitle = $html->title($options['separator'])) {
            $title[] = $pageTitle;
        }

        if ($options['reverse']) {
            $title = array_reverse($title);
        }

        if ($base) {
            array_unshift($title, $base);
        }

        return implode((string) $options['separator'], array_unique($title));
    }

}