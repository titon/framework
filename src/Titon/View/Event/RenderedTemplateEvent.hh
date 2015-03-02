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
 * The view event that occurs after the rendering of a single template type.
 *
 * @package Titon\View\Event
 */
class RenderedTemplateEvent extends Event {

    /**
     * The rendered content.
     *
     * @var string
     */
    protected string $content;

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
     * @param string $content
     * @param \Titon\View\Template $type
     */
    public function __construct(View $view, string $content, Template $type) {
        $this->view = $view;
        $this->content = $content;
        $this->type = $type;

        if ($type === Template::LAYOUT) {
            $event = 'layout';
        } else if ($type === Template::WRAPPER) {
            $event = 'wrapper';
        } else {
            $event = 'template';
        }

        parent::__construct('view.rendered.' . $event);
    }

    /**
     * Return the content.
     *
     * @return string
     */
    public function getContent(): string {
        return $this->content;
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
     * Set the content.
     *
     * @param string $content
     * @return $this
     */
    public function setContent(string $content): this {
        $this->content = $content;

        return $this;
    }

}
