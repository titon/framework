<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Cache\Storage;
use Titon\Common\Cacheable;
use Titon\Common\FactoryAware;
use Titon\Common\DataMap;
use Titon\Event\Emittable;
use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;
use Titon\Event\Subject;
use Titon\Utility\Config;
use Titon\Utility\Col;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\Utility\Registry;
use Titon\View\Exception\MissingHelperException;
use Titon\View\Exception\MissingTemplateException;
use Titon\View\Helper;

/**
 * Defines shared functionality for view managers.
 *
 * @package Titon\View\View
 */
abstract class AbstractView implements View, Listener, Subject {
    use Cacheable, Emittable, FactoryAware;

    /**
     * Variable data for templates.
     *
     * @var \Titon\Common\DataMap
     */
    protected DataMap $_data = Map {};

    /**
     * The extension used for templates.
     *
     * @var string
     */
    protected string $_extension = 'tpl';

    /**
     * List of helpers.
     *
     * @var \Titon\View\HelperMap
     */
    protected HelperMap $_helpers = Map {};

    /**
     * List of locales to use during template locating.
     *
     * @var \Titon\View\LocaleList
     */
    protected LocaleList $_locales = Vector {};

    /**
     * List of lookup paths.
     *
     * @var \Titon\View\PathList
     */
    protected PathList $_paths = Vector {};

    /**
     * Storage engine.
     *
     * @var \Titon\Cache\Storage
     */
    protected ?Storage $_storage;

    /**
     * Add lookup paths through the constructor and set the extension.
     * Furthermore, if any Titon configuration paths and locales are defined,
     * load those in.
     *
     * @param string|Traversable $paths
     * @param string $ext
     */
    public function __construct(mixed $paths, string $ext = 'tpl') {
        if ($paths) {
            $this->addPaths(Col::toVector($paths));
        }

        if ($paths = Config::get('titon.path.views')) {
            $this->addPaths(Col::toVector($paths));
        }

        if ($locales = Config::get('titon.locale.cascade')) {
            $this->addLocales(Col::toVector($locales));
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

        if ($helper instanceof Listener) {
            $this->on('view', $helper);
        }

        $this->setVariable($key, $helper);

        // Store so helpers can use helpers
        Registry::set($helper);

        return $this;
    }

    /**
     * Add a locale lookup.
     *
     * @param string $locale
     * @return $this
     */
    public function addLocale(string $locale): this {
        if (!in_array($locale, $this->_locales)) {
            $this->_locales[] = $locale;
        }

        return $this;
    }

    /**
     * Add multiple locale lookups.
     *
     * @param \Titon\View\LocaleList $locales
     * @return $this
     */
    public function addLocales(LocaleList $locales): this {
        foreach ($locales as $locale) {
            $this->addLocale($locale);
        }

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
    public function addPaths(PathList $paths): this {
        foreach ($paths as $path) {
            $this->addPath($path);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function formatPath(string $template): string {
        return trim(str_replace(['\\', '.' . $this->getExtension()], ['/', ''], $template), '/');
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
        if ($this->_helpers->contains($key)) {
            return $this->_helpers[$key];
        }

        throw new MissingHelperException(sprintf('Helper %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function getHelpers(): HelperMap {
        return $this->_helpers;
    }

    /**
     * Return all locales.
     *
     * @return \Titon\View\LocaleList
     */
    public function getLocales(): LocaleList {
        return $this->_locales;
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths(): PathList {
        return $this->_paths;
    }

    /**
     * Return the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage {
        return $this->_storage;
    }

    /**
     * {@inheritdoc}
     */
    public function getVariable(string $key): mixed {
        return $this->getVariables()->get($key);
    }

    /**
     * {@inheritdoc}
     */
    public function getVariables(): DataMap {
        return $this->_data;
    }

    /**
     * {@inheritdoc}
     */
    public function locateTemplate(string $template, Template $type = Template::OPEN): string {
        return (string) $this->cache([__METHOD__, $template, $type], (AbstractView $view) ==> {
            $template = $view->formatPath($template);
            $paths = $view->getPaths();

            $view->emit('view.locating', [&$template, $type, &$paths]);

            // Prepend parent path
            switch ($type) {
                case Template::LAYOUT:
                    $template = sprintf('private/layouts/%s', $template);
                break;
                case Template::WRAPPER:
                    $template = sprintf('private/wrappers/%s', $template);
                break;
                case Template::PARTIAL:
                    $template = sprintf('private/partials/%s', $template);
                break;
                case Template::OPEN:
                    $template = sprintf('public/%s', $template);
                break;
                case Template::CLOSED:
                    $template = sprintf('private/%s', $template);
                break;
            }

            // Generate a list of locale appended templates
            $templates = Vector {};
            $locales = $view->getLocales();
            $ext = $view->getExtension();

            if ($locales) {
                foreach ($locales as $locale) {
                    $templates[] = $template . '.' . $locale . '.' . $ext;
                }
            }

            $templates[] = $template . '.' . $ext;

            // Locate absolute path
            $absPath = '';

            foreach ($paths as $path) {
                if ($absPath) {
                    break;
                }

                foreach ($templates as $template) {
                    if (file_exists($path . $template)) {
                        $absPath = $path . $template;
                        break;
                    }
                }
            }

            if (!$absPath) {
                throw new MissingTemplateException(sprintf('View template `%s` does not exist', $template));
            }

            $view->emit('view.located', [&$absPath, $type]);

            return $absPath;
        });
    }

    /**
     * Triggered before all templates are rendered in render().
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string $template
     */
    public function preRender(Event $event, View $view, string $template): void {
        return;
    }

    /**
     * Triggered after all templates are rendered in render().
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string $response
     */
    public function postRender(Event $event, View $view, string $response): void {
        return;
    }

    /**
     * Register the events to listen to.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function registerEvents(): ListenerMap {
        return Map {
            'view.rendering' => Map {'method' => 'preRender', 'priority' => 1},
            'view.rendered' => Map {'method' => 'postRender', 'priority' => 1}
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
     * Set a list of locales and overwrite any previously defined paths.
     *
     * @param \Titon\View\LocaleList $locales
     * @return $this
     */
    public function setLocales(LocaleList $locales): this {
        $this->_locales = $locales;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setPaths(PathList $paths): this {
        $this->_paths = $paths;

        return $this;
    }

    /**
     * Set the storage engine to cache views.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage): this {
        $this->_storage = $storage;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariable(string $key, mixed $value): this {
        $this->_data[Inflector::variable($key)] = $value;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariables(DataMap $data): this {
        foreach ($data as $key => $value) {
            $this->setVariable($key, $value);
        }

        return $this;
    }

}