<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Locator;

use Titon\Utility\Config;
use Titon\Utility\Col;
use Titon\Utility\Path;
use Titon\View\Exception\MissingTemplateException;
use Titon\View\Locator;
use Titon\View\Template;
use Titon\View\PathList;

abstract class AbstractLocator implements Locator {

    /**
     * The extension used for templates.
     *
     * @var string
     */
    protected string $extension = 'tpl';

    /**
     * List of lookup paths.
     *
     * @var \Titon\View\PathList
     */
    protected PathList $paths = Vector {};

    public function __construct(mixed $paths, string $ext = 'tpl') {
        if ($paths) {
            $this->addPaths(Col::toVector($paths));
        }

        if ($paths = Config::get('titon.path.views')) {
            $this->addPaths(Col::toVector($paths));
        }

        $this->setExtension($ext);
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
        return trim(str_replace(['.' . $this->getExtension(), '\\'], ['', '/'], $template), '/');
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
    public function getPaths(): PathList {
        return $this->paths;
    }

    /**
     * {@inheritdoc}
     */
    <<__Memoize>>
    public function locate(string $template, Template $type = Template::OPEN): string {
        $template = $this->formatPath($template);
        $paths = $this->getPaths();

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
        //$locales = $this->getLocales();
        $ext = $this->getExtension();

        //if ($locales) {
        //    foreach ($locales as $locale) {
        //        $templates[] = $template . '.' . $locale . '.' . $ext;
        //   }
        //}

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

        return $absPath;
    }

    /**
     * {@inheritdoc}
     */
    public function setExtension(string $ext): this {
        $this->extension = $ext;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setPaths(PathList $paths): this {
        $this->paths = $paths;

        return $this;
    }

}
