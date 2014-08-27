<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\View;

use Titon\Cache\Storage;
use Titon\Common\Attachable;
use Titon\Common\Cacheable;
use Titon\Common\FactoryAware;
use Titon\Event\Emittable;
use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Utility\Config;
use Titon\Utility\Converter;
use Titon\Utility\Traverse;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Registry;
use Titon\View\Exception\MissingHelperException;
use Titon\View\Exception\MissingTemplateException;
use Titon\View\Helper;
use Titon\View\View;
use \Traversable;

/**
 * Defines shared functionality for view managers.
 *
 * @package Titon\View\View
 */
abstract class AbstractView implements View, Listener {
    use Attachable, Cacheable, Emittable, FactoryAware;

    /**
     * Variable data for templates.
     *
     * @type Map<string, ?mixed>
     */
    protected Map<string, ?mixed> $_data = Map {};

    /**
     * The extension used in templates.
     *
     * @type string
     */
    protected string $_extension = 'tpl';

    /**
     * List of helpers.
     *
     * @type Map<string, Helper>
     */
    protected Map<string, Helper> $_helpers = Map {};

    /**
     * List of template lookup paths.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_paths = Vector {};

    /**
     * Storage engine.
     *
     * @type \Titon\Cache\Storage
     */
    protected ?Storage $_storage;

    /**
     * Add paths through the constructor.
     *
     * @param array|string $paths
     * @param string $ext
     */
    public function __construct(mixed $paths, string $ext = 'tpl') {
        if ($paths) {
            $this->addPaths(Converter::toVector($paths));
        }

        if ($configPaths = Config::get('titon.path.views')) {
            $this->addPaths(Converter::toVector($configPaths));
        }

        if ($ext) {
            $this->setExtension($ext);
        }

        $this->on('view', $this);
    }

    /**
     * {@inheritdoc}
     */
    public function addHelper(string $key, Helper $helper): this {
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
    public function addPath(string $path): this {
        $this->_paths[] = Path::ds($path, true);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addPaths(Vector<string> $paths): this {
        foreach ($paths as $path) {
            $this->addPath($path);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function formatPath(mixed $template): string {
        if ($template instanceof Traversable) {
            $template = new Map($template);
        }

        if ($template instanceof Collection) {
            $ext = isset($template['ext']) ? $template['ext'] : null;

            unset($template['ext'], $template['locale']);

            $template = implode('/', Traverse::filter($template, false, function($value) {
                return ((is_string($value) || is_numeric($value)) && $value);
            }));

            if ($ext) {
                $template .= '.' . $ext;
            }
        }

        // Replace \ with / in case we are running on unix
        // Also remove the template extension, but not the type extension
        return str_replace(['\\', '.' . $this->getExtension()], ['/', ''], $template);
    }

    /**
     * {@inheritdoc}
     */
    public function getExtension(): string {
        return $this->_extension;
    }

    /**
     * {@inheritdoc}
     */
    public function getHelper(string $key): Helper {
        if (isset($this->_helpers[$key])) {
            return $this->_helpers[$key];
        }

        throw new MissingHelperException(sprintf('Helper %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function getHelpers(): Map<string, Helper> {
        return $this->_helpers;
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths(): Vector<string> {
        return $this->_paths;
    }

    /**
     * {@inheritdoc}
     */
    public function getStorage(): ?Storage {
        return $this->_storage;
    }

    /**
     * {@inheritdoc}
     */
    public function getVariable(string $key): ?mixed {
        return isset($this->_data[$key]) ? $this->_data[$key] : null;
    }

    /**
     * {@inheritdoc}
     */
    public function getVariables(): Map<string, ?mixed> {
        return $this->_data;
    }

    /**
     * {@inheritdoc}
     */
    public function locateTemplate(mixed $template, int $type = self::TEMPLATE): string {
        return $this->cache([__METHOD__, $template, $type], function() use ($template, $type) {
            $paths = $this->getPaths();

            $this->emit('view.preLocate', [&$template, $type, &$paths]);

            // Combine path parts
            $template = $this->formatPath($template);

            // Determine parent path
            switch ($type) {
                case self::LAYOUT:
                    $template = sprintf('private/layouts/%s', $template);
                break;

                case self::WRAPPER:
                    $template = sprintf('private/wrappers/%s', $template);
                break;

                case self::PARTIAL:
                    $template = sprintf('private/partials/%s', $template);
                break;

                case self::TEMPLATE:
                    $template = sprintf('public/%s', $template);
                break;

                case self::PRIVATE_TEMPLATE:
                default:
                    $template = sprintf('private/%s', $template);
                break;
            }

            // Fetch locales to loop through
            $templates = Vector {};
            $locales = Config::get('titon.locale.cascade') ?: Vector {};
            $ext = $this->getExtension();

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
    public function preRender(Event $event, View $view, mixed &$template): void {
        return;
    }

    /**
     * Triggered after all templates are rendered in render().
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string $response
     */
    public function postRender(Event $event, View $view, string &$response): void {
        return;
    }

    /**
     * Register the events to listen to.
     *
     * @return Map<string, mixed>
     */
    public function registerEvents(): Map<string, mixed> {
        return Map {
            'view.preRender' => Map {'method' => 'preRender', 'priority' => 1},
            'view.postRender' => Map {'method' => 'postRender', 'priority' => 1}
        };
    }

    /**
     * {@inheritdoc}
     */
    public function setExtension(string $ext): this {
        $this->_extension = $ext;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setStorage(Storage $storage): this {
        $this->_storage = $storage;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariable(string $key, ?mixed $value): this {
        $this->_data[Inflector::variable($key)] = $value;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariables(Map<string, ?mixed> $data): this {
        foreach ($data as $key => $value) {
            $this->setVariable($key, $value);
        }

        return $this;
    }

}