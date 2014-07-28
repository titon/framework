<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Utility\Config;
use Titon\Utility\Registry;

/**
 * The AssetHelper aids in the process of including external stylesheets and scripts.
 *
 * @package Titon\View\Helper
 * @property \Titon\View\Helper\HtmlHelper $html
 */
class AssetHelper extends AbstractHelper {

    /**
     * Default locations.
     */
    const string HEADER = 'header';
    const string FOOTER = 'footer';

    /**
     * Configuration.
     *
     * @type Map<string, mixed> {
     *      @type bool $timestamp   Add a file timestamp to every asset URL
     *      @type string $webroot   Path to the webroot to use for file checking
     * }
     */
    protected Map<string, mixed> $_config = Map {
        'timestamp' => true,
        'webroot' => ''
    };

    /**
     * A list of JavaScript files to include in the current page.
     *
     * @type Map<string, Map<int, Map<string, mixed>>>
     */
    protected Map<string, Map<int, Map<string, mixed>>> $_scripts = Map {};

    /**
     * A list of CSS stylesheets to include in the current page.
     *
     * @type Map<int, Map<string, mixed>>
     */
    protected Map<int, Map<string, mixed>> $_stylesheets = Map {};

    /**
     * Add a JavaScript file to the current page request.
     *
     * @param string $script
     * @param string $location
     * @param int $order
     * @param string $env
     * @return $this
     */
    public function addScript(string $script, string $location = self::FOOTER, int $order = 0, string $env = ''): this {
        if (!isset($this->_scripts[$location])) {
            $this->_scripts[$location] = Map {};
        }

        if ($order === 0) {
            $order = count($this->_scripts[$location]);
        }

        while (isset($this->_scripts[$location][$order])) {
            $order++;
        }

        $this->_scripts[$location][$order] = Map {
            'path' => $this->preparePath($script, 'js'),
            'env' => $env
        };

        return $this;
    }

    /**
     * Add a CSS stylesheet to the current page request.
     *
     * @param string $sheet
     * @param Map<string, mixed> $attributes
     * @param int $order
     * @param int $env
     * @return $this
     */
    public function addStylesheet(string $sheet, Map<string, mixed> $attributes = Map {}, int $order = 0, string $env = ''): this {
        if ($order === 0) {
            $order = count($this->_stylesheets);
        }

        while (isset($this->_stylesheets[$order])) {
            $order++;
        }

        $this->_stylesheets[$order] = Map {
            'path' => $this->preparePath($sheet, 'css'),
            'attributes' => $attributes,
            'env' => $env
        };

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
    public function preparePath(string $path, string $ext): string {

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
    public function initialize(): void {
        $this->attachObject('html', function() {
            return Registry::factory('Titon\View\Helper\HtmlHelper');
        });
    }

    /**
     * Return all the attached scripts. Uses the HTML helper to build the HTML tags.
     *
     * @param string $location
     * @param string $env
     * @return string
     */
    public function scripts(string $location = self::FOOTER, string $env = ''): string {
        $output = '';

        if (!empty($this->_scripts[$location])) {
            $scripts = $this->_scripts[$location];
            ksort($scripts);

            foreach ($scripts as $script) {
                if ($script['env'] === '' || $script['env'] === $env) {
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
    public function stylesheets(string $env = ''): string {
        $output = '';

        if ($this->_stylesheets) {
            $stylesheets = $this->_stylesheets;
            ksort($stylesheets);

            foreach ($stylesheets as $sheet) {
                if ($sheet['env'] === '' || $sheet['env'] === $env) {
                    $output .= $this->html->link($sheet['path'], $sheet['attributes']);
                }
            }
        }

        return $output;
    }

}