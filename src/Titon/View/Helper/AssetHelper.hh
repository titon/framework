<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Utility\Path;
use Titon\View\Exception\InvalidWebrootException;

type Asset = shape('path' => string, 'env' => string, 'attributes' => AttributeMap);
type ScriptMap = Map<string, Map<int, Asset>>;
type StyleSheetMap = Map<int, Asset>;

/**
 * The AssetHelper aids in the process of including external stylesheets and scripts.
 *
 * @package Titon\View\Helper
 * @property \Titon\View\Helper\HtmlHelper $html
 */
class AssetHelper extends AbstractHelper {

    /**
     * Provides asset timestamping for cache busting.
     *
     * @type bool
     */
    protected bool $_timestamping = true;

    /**
     * A list of JavaScript files to include in the current page.
     *
     * @type \Titon\View\Helper\ScriptMap
     */
    protected ScriptMap $_scripts = Map {};

    /**
     * A list of CSS stylesheets to include in the current page.
     *
     * @type \Titon\View\Helper\StyleSheetMap
     */
    protected StyleSheetMap $_stylesheets = Map {};

    /**
     * Path to the webroot where assets reside.
     *
     * @type string
     */
    protected string $_webroot = '';

    /**
     * Set the webroot and timestamping configuration.
     *
     * @param string $webroot
     * @param bool $timestamp
     */
    public function __construct(string $webroot, bool $timestamp = true) {
        $this->setWebroot($webroot);
        $this->setTimestamping($timestamp);
    }

    /**
     * Add a JavaScript file to the current page request.
     *
     * @param string $script
     * @param string $location
     * @param int $order
     * @param string $env
     * @return $this
     */
    public function addScript(string $script, string $location = 'footer', int $order = 0, string $env = ''): this {
        $scripts = $this->getScripts();

        if (!$scripts->contains($location)) {
            $this->_scripts[$location] = Map {};
        }

        if ($order === 0) {
            $order = count($scripts[$location]);
        }

        while ($scripts[$location]->contains($order)) {
            $order++;
        }

        $this->_scripts[$location][$order] = shape(
            'path' => $this->preparePath($script, 'js'),
            'attributes' => Map {},
            'env' => $env
        );

        return $this;
    }

    /**
     * Add a CSS stylesheet to the current page request.
     *
     * @param string $sheet
     * @param \Titon\View\Helper\AttributeMap $attributes
     * @param int $order
     * @param int $env
     * @return $this
     */
    public function addStyleSheet(string $sheet, AttributeMap $attributes = Map {}, int $order = 0, string $env = ''): this {
        $stylesheets = $this->getStyleSheets();

        if ($order === 0) {
            $order = count($stylesheets);
        }

        while ($stylesheets->contains($order)) {
            $order++;
        }

        $this->_stylesheets[$order] = shape(
            'path' => $this->preparePath($sheet, 'css'),
            'attributes' => $attributes,
            'env' => $env
        );

        return $this;
    }

    /**
     * Return the list of defined scripts.
     *
     * @return \Titon\View\Helper\ScriptMap
     */
    public function getScripts(): ScriptMap {
        return $this->_scripts;
    }

    /**
     * Return the list of defined stylesheets.
     *
     * @return \Titon\View\Helper\StyleSheetMap
     */
    public function getStyleSheets(): StyleSheetMap {
        return $this->_stylesheets;
    }

    /**
     * Return the asset webroot.
     *
     * @return string
     */
    public function getWebroot(): string {
        return $this->_webroot;
    }

    /**
     * Return true if asset timestamping is enabled.
     *
     * @return bool
     */
    public function isTimestamping(): bool {
        return $this->_timestamping;
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

        // Remove query string
        $parts = explode('?', $path);
        $path = $parts[0];
        $query = array_key_exists(1, $parts) ? $parts[1] : '';

        // Apply extension
        if (substr($path, -(strlen($ext) + 1)) !== '.' . $ext) {
            $path .= '.' . $ext;
        }

        // Apply query
        if ($query) {
            $path .= '?' . $query;
        }

        // Apply timestamp
        if ($this->isTimestamping()) {
            $absPath = $this->getWebroot() . $path;

            if (file_exists($absPath)) {
                $path .= ($query ? '&' : '?') . filemtime($absPath);
            }
        }

        return $path;
    }

    /**
     * Enable or disable asset timestamping.
     *
     * @param bool $status
     * @return $this
     */
    public function setTimestamping(bool $status): this {
        $this->_timestamping = $status;

        return $this;
    }

    /**
     * Set the webroot path.
     *
     * @param string $path
     * @return $this
     */
    public function setWebroot(string $path): this {
        $path = Path::ds($path, true);

        if (!file_exists($path)) {
            throw new InvalidWebrootException('Asset webroot does not exist');
        }

        $this->_webroot = $path;

        return $this;
    }

    /**
     * Return all the attached scripts. Uses the HTML helper to build the HTML tags.
     *
     * @param string $location
     * @param string $env
     * @return string
     */
    public function scripts(string $location = 'footer', string $env = ''): string {
        $scripts = $this->getScripts();
        $output = '';

        /** @type \Titon\View\Helper\HtmlHelper $html */
        $html = $this->getHelper('html');

        if ($scripts->contains($location)) {
            $groupedScripts = $scripts[$location];
            ksort($groupedScripts);

            foreach ($groupedScripts as $script) {
                if ($script['env'] === '' || $script['env'] === $env) {
                    $output .= $html->script($script['path']);
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
        $stylesheets = $this->getStyleSheets();
        $output = '';

        /** @type \Titon\View\Helper\HtmlHelper $html */
        $html = $this->getHelper('html');

        if ($stylesheets) {
            ksort($stylesheets);

            foreach ($stylesheets as $sheet) {
                if ($sheet['env'] === '' || $sheet['env'] === $env) {
                    $output .= $html->link($sheet['path'], $sheet['attributes']);
                }
            }
        }

        return $output;
    }

}