<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Html;

use Titon\Utility\Config;
use Titon\Utility\Registry;
use Titon\View\Helper\AbstractHelper;

/**
 * The AssetHelper aids in the process of including external stylesheets and scripts.
 *
 * @package Titon\View\Helper\Html
 * @property \Titon\View\Helper\Html\HtmlHelper $html
 */
class AssetHelper extends AbstractHelper {

    /**
     * Default locations.
     */
    const HEADER = 'header';
    const FOOTER = 'footer';

    /**
     * Configuration.
     *
     * @type array {
     *      @type bool $timestamp   Add a file timestamp to every asset URL
     *      @type string $webroot   Path to the webroot to use for file checking
     * }
     */
    protected $_config = [
        'timestamp' => true,
        'webroot' => ''
    ];

    /**
     * A list of JavaScript files to include in the current page.
     *
     * @type array
     */
    protected $_scripts = [];

    /**
     * A list of CSS stylesheets to include in the current page.
     *
     * @type array
     */
    protected $_stylesheets = [];

    /**
     * Add a JavaScript file to the current page request.
     *
     * @param string $script
     * @param string $location
     * @param int $order
     * @param int $env
     * @return $this
     */
    public function addScript($script, $location = self::FOOTER, $order = null, $env = null) {
        if (!isset($this->_scripts[$location])) {
            $this->_scripts[$location] = [];
        }

        if (!is_numeric($order)) {
            $order = count($this->_scripts[$location]);
        }

        while (isset($this->_scripts[$location][$order])) {
            $order++;
        }

        $this->_scripts[$location][$order] = [
            'path' => $this->preparePath($script, 'js'),
            'env' => $env
        ];

        return $this;
    }

    /**
     * Add a CSS stylesheet to the current page request.
     *
     * @param string $sheet
     * @param array $attributes
     * @param int $order
     * @param int $env
     * @return $this
     */
    public function addStylesheet($sheet, array $attributes = [], $order = null, $env = null) {
        if (!is_numeric($order)) {
            $order = count($this->_stylesheets);
        }

        while (isset($this->_stylesheets[$order])) {
            $order++;
        }

        $this->_stylesheets[$order] = [
            'path' => $this->preparePath($sheet, 'css'),
            'attributes' => $attributes,
            'env' => $env
        ];

        return $this;
    }

    /**
     * Prepare an asset file path by appending the extension if it is missing,
     * and appending a last modification timestamp. Take query strings into account.
     *
     * @param string $path
     * @param string $ext
     * @return string
     */
    public function preparePath($path, $ext) {

        // Don't modify external assets
        if (substr($path, 0, 2) === '//' || preg_match('/^https?:/i', $path)) {
            return $path;
        }

        $parts = explode('?', $path);
        $path = $parts[0];
        $query = isset($parts[1]) ? $parts[1] : null;

        // Apply extension
        if (substr($path, -(strlen($ext) + 1)) !== '.' . $ext) {
            $path .= '.' . $ext;
        }

        // Determine system path via webroot in the request
        $root = $this->getConfig('webroot');

        if (!$root && Config::has('titon.webroot')) {
            $root = Config::get('titon.webroot');
        }

        $systemPath = $root . $path;

        // Apply query
        if ($query) {
            $path .= '?' . $query;
        }

        // Apply timestamp
        if ($this->getConfig('timestamp') && file_exists($systemPath)) {
            $path .= ($query ? '&' : '?') . filemtime($systemPath);
        }

        return $path;
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
     * Return all the attached scripts. Uses the HTML helper to build the HTML tags.
     *
     * @param string $location
     * @param string $env
     * @return string
     */
    public function scripts($location = self::FOOTER, $env = null) {
        $output = null;

        if (!empty($this->_scripts[$location])) {
            $scripts = $this->_scripts[$location];
            ksort($scripts);

            foreach ($scripts as $script) {
                if ($script['env'] === null || $script['env'] === $env) {
                    $output .= $this->html->script($script['path']);
                }
            }
        }

        return $output;
    }

    /**
     * Return all the attached stylesheets. Uses the HTML helper to build the HTML tags.
     *
     * @param string $env
     * @return string
     */
    public function stylesheets($env = null) {
        $output = null;

        if ($this->_stylesheets) {
            $stylesheets = $this->_stylesheets;
            ksort($stylesheets);

            foreach ($stylesheets as $sheet) {
                if ($sheet['env'] === null || $sheet['env'] === $env) {
                    $output .= $this->html->link($sheet['path'], $sheet['attributes']);
                }
            }
        }

        return $output;
    }

}