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
abstract class AbstractView implements View, Subject {
    use Cacheable, Emittable, FactoryAware;

    /**
     * Variable data for templates.
     *
     * @var \Titon\Common\DataMap
     */
    protected DataMap $data = Map {};

    /**
     * The extension used for templates.
     *
     * @var string
     */
    protected string $extension = 'tpl';

    /**
     * List of helpers.
     *
     * @var \Titon\View\HelperMap
     */
    protected HelperMap $helpers = Map {};

    /**
     * List of locales to use during template locating.
     *
     * @var \Titon\View\LocaleList
     */
    protected LocaleList $locales = Vector {};

    /**
     * List of lookup paths.
     *
     * @var \Titon\View\PathList
     */
    protected PathList $paths = Vector {};

    /**
     * Storage engine.
     *
     * @var \Titon\Cache\Storage
     */
    protected ?Storage $storage;

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
    }

    /**
     * {@inheritdoc}
     */
    public function addHelper(string $key, Helper $helper): this {
        $helper->setView($this);

        $this->helpers[$key] = $helper;

        if ($helper instanceof Listener) {
            $this->on($helper);
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
        if (!in_array($locale, $this->locales)) {
            $this->locales[] = $locale;
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
        $this->paths[] = Path::ds($path, true);

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
        return $this->extension;
    }

    /**
     * {@inheritdoc}
     */
    public function getHelper(string $key): Helper {
        if ($this->helpers->contains($key)) {
            return $this->helpers[$key];
        }

        throw new MissingHelperException(sprintf('Helper %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function getHelpers(): HelperMap {
        return $this->helpers;
    }

    /**
     * Return all locales.
     *
     * @return \Titon\View\LocaleList
     */
    public function getLocales(): LocaleList {
        return $this->locales;
    }

    /**
     * {@inheritdoc}
     */
    public function getPaths(): PathList {
        return $this->paths;
    }

    /**
     * Return the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage {
        return $this->storage;
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
        return $this->data;
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
     * {@inheritdoc}
     */
    public function setExtension(string $ext): this {
        $this->extension = $ext;

        return $this;
    }

    /**
     * Set a list of locales and overwrite any previously defined paths.
     *
     * @param \Titon\View\LocaleList $locales
     * @return $this
     */
    public function setLocales(LocaleList $locales): this {
        $this->locales = $locales;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setPaths(PathList $paths): this {
        $this->paths = $paths;

        return $this;
    }

    /**
     * Set the storage engine to cache views.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage): this {
        $this->storage = $storage;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setVariable(string $key, mixed $value): this {
        $this->data[Inflector::variable($key)] = $value;

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
