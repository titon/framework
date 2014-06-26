<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\View;

use Titon\Cache\StorageAware;
use Titon\Common\Base;
use Titon\Common\Attachable;
use Titon\Common\Cacheable;
use Titon\Common\FactoryAware;
use Titon\Event\Emittable;
use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Utility\Config;
use Titon\Utility\Hash;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Registry;
use Titon\View\Exception\MissingHelperException;
use Titon\View\Exception\MissingTemplateException;
use Titon\View\Helper;
use Titon\View\View;

/**
 * Defines shared functionality for view managers.
 *
 * @package Titon\View\View
 */
abstract class AbstractView extends Base implements View, Listener {
    use Attachable, Cacheable, Emittable, FactoryAware, StorageAware;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $extension     The extension used in templates
     * }
     */
    protected $_config = [
        'extension' => 'tpl'
    ];

    /**
     * Variable data for templates.
     *
     * @type array
     */
    protected $_data = [];

    /**
     * List of helpers.
     *
     * @type \Titon\View\Helper[]
     */
    protected $_helpers = [];

    /**
     * List of template lookup paths.
     *
     * @type array
     */
    protected $_paths = [];

    /**
     * Add paths through the constructor.
     *
     * @param array|string $paths
     * @param array $config
     */
    public function __construct($paths = [], array $config = []) {
        parent::__construct($config);

        if ($paths) {
            $this->addPaths((array) $paths);
        }

        $this->on('view', $this);
    }

    /**
     * {@inheritdoc}
     */
    public function addHelper($key, Helper $helper) {
        $helper->setView($this);

        $this->_helpers[$key] = $helper;
        $this->_attached[$key] = $helper;

        if ($helper instanceof Listener) {
            $this->on('view', $helper);
        }

        $this->setVariable($key, $helper);

        // Store so helpers can use helpers
        Registry::set($helper);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addPath($path) {
        $this->_paths[] = $path;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addPaths(array $paths) {
        foreach ($paths as $path) {
            $this->addPath($path);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function formatPath($template) {
        if (is_array($template)) {
            $ext = isset($template['ext']) ? $template['ext'] : null;

            unset($template['ext'], $template['locale']);

            $template = implode('/', Hash::filter($template, false, function($value) {
                return ((is_string($value) || is_numeric($value)) && $value);
            }));

            if ($ext) {
                $template .= '.' . $ext;
            }
        }

        // Replace \ with / in case we are running on unix
        // Also remove the template extension, but not the type extension
        return str_replace(['\\', '.' . $this->getConfig('extension')], ['/', ''], $template);
    }

    /**
     * {@inheritdoc}
     */
    public function getHelper($key) {
        if (isset($this->_helpers[$key])) {
            return $this->_helpers[$key];
        }

        throw new MissingHelperException(sprintf('Helper %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function getHelpers() {
        return $this->_helpers;
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths() {
        return $this->_paths;
    }

    /**
     * {@inheritdoc}
     */
    public function getVariable($key) {
        return isset($this->_data[$key]) ? $this->_data[$key] : null;
    }

    /**
     * {@inheritdoc}
     */
    public function getVariables() {
        return $this->_data;
    }

    /**
     * {@inheritdoc}
     */
    public function locateTemplate($template, $type = self::TEMPLATE) {
        return $this->cache([__METHOD__, $template, $type], function() use ($template, $type) {
            $paths = $this->getPaths();

            if ($configPaths = Config::get('titon.path.views')) {
                $paths = array_merge($paths, $configPaths);
            }

            $this->emit('view.preLocate', [&$template, $type, &$paths]);

            if (!$paths) {
                throw new MissingTemplateException('No template lookup paths have been defined');
            }

            // Combine path parts
            $template = $this->formatPath($template);

            // Determine parent path
            switch ($type) {
                case self::LAYOUT:
                    $template = sprintf('/private/layouts/%s', $template);
                break;

                case self::WRAPPER:
                    $template = sprintf('/private/wrappers/%s', $template);
                break;

                case self::PARTIAL:
                    $template = sprintf('/private/partials/%s', $template);
                break;

                case self::TEMPLATE:
                    $template = sprintf('/public/%s', $template);
                break;

                case self::PRIVATE_TEMPLATE:
                default:
                    $template = sprintf('/private/%s', $template);
                break;
            }

            // Fetch locales to loop through
            $templates = [];
            $locales = Config::get('titon.locale.cascade') ?: [];
            $ext = $this->getConfig('extension');

            if ($locales) {
                foreach ($locales as $locale) {
                    $templates[] = $template . '.' . $locale . '.' . $ext;
                }
            }

            $templates[] = $template . '.' . $ext;

            $this->emit('view.postLocate', [&$templates, $type, &$paths]);

            // Locate absolute path
            foreach ($paths as $path) {
                foreach ($templates as $template) {
                    if (file_exists($path . $template)) {
                        return Path::ds($path . $template);
                    }
                }
            }

            throw new MissingTemplateException(sprintf('View template %s does not exist', Path::alias(rtrim($paths[0], DIRECTORY_SEPARATOR) . $template)));
        });
    }

    /**
     * Triggered before all templates are rendered in render().
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string|array $template
     */
    public function preRender(Event $event, View $view, &$template) {
        return;
    }

    /**
     * Triggered after all templates are rendered in render().
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string $response
     */
    public function postRender(Event $event, View $view, &$response) {
        return;
    }

    /**
     * Register the events to listen to.
     *
     * @return array
     */
    public function registerEvents() {
        return [
            'view.preRender' => ['method' => 'preRender', 'priority' => 1],
            'view.postRender' => ['method' => 'postRender', 'priority' => 1]
        ];
    }

    /**
     * {@inheritdoc}
     */
    public function setVariable($key, $value) {
        $this->_data[Inflector::variable($key)] = $value;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariables(array $data) {
        foreach ($data as $key => $value) {
            $this->setVariable($key, $value);
        }

        return $this;
    }

}