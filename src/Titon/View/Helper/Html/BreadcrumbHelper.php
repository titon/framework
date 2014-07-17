<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Html;

use Titon\Utility\Hash;
use Titon\Utility\Registry;
use Titon\View\Helper\AbstractHelper;

/**
 * The BreadcrumbHelper is primarily used for adding and generating breadcrumb lists.
 *
 * @package Titon\View\Helper\Html
 * @property \Titon\View\Helper\Html\HtmlHelper $html
 */
class BreadcrumbHelper extends AbstractHelper {

    /**
     * A list of all breadcrumbs in the trail, with the title, url and attributes.
     *
     * @type array
     */
    protected $_breadcrumbs = [];

    /**
     * Add a link to the breadcrumbs.
     *
     * @param string|array $titles
     * @param string|array $url
     * @param array $attributes
     * @return $this
     */
    public function add($titles, $url = null, array $attributes = []) {
        if (is_array($titles)) {
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
     * @return array
     */
    public function all() {
        return $this->_breadcrumbs;
    }

    /**
     * Add a breadcrumb to the end of the list.
     *
     * @param string $title
     * @param string|array $url
     * @param array $attributes
     * @return $this
     */
    public function append($title, $url, array $attributes = []) {
        $this->_breadcrumbs[] = [
            'title' => $title,
            'url' => $url,
            'attributes' => $attributes
        ];

        return $this;
    }

    /**
     * Return the first crumb in the list.
     *
     * @return mixed
     */
    public function first() {
        $crumbs = $this->_breadcrumbs;

        if (!$crumbs) {
            return null;
        }

        $first = array_slice($crumbs, 0, 1);

        return $first[0];
    }

    /**
     * Return an array of breadcrumbs formatted as anchor links.
     *
     * @param array $attributes
     * @return array
     */
    public function generate(array $attributes = []) {
        $trail = [];

        if ($this->_breadcrumbs) {
            foreach ($this->_breadcrumbs as $crumb) {
                $trail[] = $this->html->anchor($crumb['title'], $crumb['url'], $crumb['attributes'] + $attributes);
            }
        }

        return $trail;
    }

    /**
     * Attach the HtmlHelper.
     */
    public function initialize() {
        $this->attachObject('html', function() {
            return Registry::factory('Titon\View\Helper\Html\HtmlHelper');
        });
    }

    /**
     * Return the last crumb in the list.
     *
     * @return mixed
     */
    public function last() {
        $crumbs = $this->_breadcrumbs;

        if (!$crumbs) {
            return null;
        }

        $last = array_slice($crumbs, -1);

        return $last[0];
    }

    /**
     * Add a breadcrumb to the beginning of the list.
     *
     * @param string $title
     * @param string|array $url
     * @param array $attributes
     * @return $this
     */
    public function prepend($title, $url, array $attributes = []) {
        array_unshift($this->_breadcrumbs, [
            'title' => $title,
            'url' => $url,
            'attributes' => $attributes
        ]);

        return $this;
    }

    /**
     * Generate a page title based off the current crumbs.
     *
     * @param string $base
     * @param array $options
     * @return string
     */
    public function title($base = '', array $options = []) {
        $options = $options + [
            'reverse' => false,
            'depth' => 3,
            'separator' => ' - '
        ];

        $crumbs = Hash::pluck($this->_breadcrumbs, 'title');
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