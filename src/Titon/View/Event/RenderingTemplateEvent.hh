<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Event;

use Titon\Event\Event;
use Titon\View\View;
use Titon\View\Template;

/**
 * The view event that occurs before the rendering of a single template type.
 *
 * @package Titon\View\Event
 */
class RenderingTemplateEvent extends Event {

    /**
     * The template path to render.
     *
     * @var string
     */
    protected string $template;

    /**
     * The type of template.
     *
     * @var \Titon\View\Template
     */
    protected Template $type;

    /**
     * The view instance.
     *
     * @var \Titon\View\View
     */
    protected View $view;

    /**
     * Store the event settings.
     *
     * @param \Titon\View\View $view
     * @param string $template
     * @param \Titon\View\Template $type
     */
    public function __construct(View $view, string $template, Template $type) {
        $this->view = $view;
        $this->template = $template;
        $this->type = $type;

        if ($type === Template::LAYOUT) {
            $event = 'layout';
        } else if ($type === Template::WRAPPER) {
            $event = 'wrapper';
        } else {
            $event = 'template';
        }

        parent::__construct('view.rendering.' . $event);
    }

    /**
     * Return the template path.
     *
     * @return string
     */
    public function getTemplate(): string {
        return $this->template;
    }

    /**
     * Return the template type.
     *
     * @return \Titon\View\Template
     */
    public function getType(): Template {
        return $this->type;
    }

    /**
     * Return the view instance.
     *
     * @return \Titon\View\View
     */
    public function getView(): View {
        return $this->view;
    }

    /**
     * Set the template.
     *
     * @param string $template
     * @return $this
     */
    public function setTemplate(string $template): this {
        $this->template = $template;

        return $this;
    }

}
