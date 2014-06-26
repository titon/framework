<?php
namespace Titon\View\Helper\Xhtml;

use Titon\Utility\Registry;
use Titon\Test\TestCase;
use Titon\View\View\TemplateView;

/**
 * @property \Titon\View\Helper\Xhtml\BreadcrumbHelper $object
 */
class BreadcrumbHelperTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new BreadcrumbHelper();
    }

    public function testOneCrumb() {
        $this->object->add('Title', '/');

        $this->assertEquals([
            '<a href="/">Title</a>' . PHP_EOL
        ], $this->object->generate());
    }

    public function testMultipleCrumbs() {
        $this->object
            ->add('Title', '/')
            ->add('Title 2', '/static/url', ['class' => 'tier2'])
            ->add('Title 3', '/main/index/view/123', ['class' => 'tier3']);

        $this->assertEquals([
            '<a href="/">Title</a>' . PHP_EOL,
            '<a class="tier2" href="/static/url">Title 2</a>' . PHP_EOL,
            '<a class="tier3" href="/main/index/view/123">Title 3</a>' . PHP_EOL
        ], $this->object->generate());
    }

    public function testFirstList() {
        $this->assertEquals(null, $this->object->first());
        $this->assertEquals(null, $this->object->last());

        $this->object
            ->add('Title', '/')
            ->add('Title 2', '/static/url', ['class' => 'tier2'])
            ->add('Title 3', ['action' => 'view', 123], ['class' => 'tier3']);

        $this->assertEquals([
            'title' => 'Title',
            'url' => '/',
            'attributes' => []
        ], $this->object->first());

        $this->assertEquals([
            'title' => 'Title 3',
            'url' => ['action' => 'view', 123],
            'attributes' =>['class' => 'tier3']
        ], $this->object->last());
    }

    public function testAppendPrepend() {
        $this->object->add('Base', '/');

        $this->assertEquals([
            [
                'title' => 'Base',
                'url' => '/',
                'attributes' => []
            ]
        ], $this->object->all());

        $this->object->prepend('Before', '/');
        $this->object->append('After', '/');

        $this->assertEquals([
            [
                'title' => 'Before',
                'url' => '/',
                'attributes' => []
            ], [
                'title' => 'Base',
                'url' => '/',
                'attributes' => []
            ], [
                'title' => 'After',
                'url' => '/',
                'attributes' => []
            ]
        ], $this->object->all());
    }

    public function testTitle() {
        $this->object->add([
            'A' => '/a',
            'B' => '/b',
            'C' => '/c',
            'D' => '/d',
            'E' => '/e',
            'F' => '/f',
            'G' => '/g'
        ]);

        $this->assertEquals('A - E - F - G', $this->object->title());
        $this->assertEquals('Base - A - E - F - G', $this->object->title('Base'));
        $this->assertEquals('Base - A - C - D - E - F - G', $this->object->title('Base', ['depth' => 5]));
        $this->assertEquals('Base - A - G', $this->object->title('Base', ['depth' => 1]));
        $this->assertEquals('Base - G - F - E - A', $this->object->title('Base', ['reverse' => true]));
        $this->assertEquals('G > F > E > A', $this->object->title(null, ['reverse' => true, 'separator' => ' > ']));
        $this->assertEquals('Base - A - B - C - D - E - F - G', $this->object->title('Base', ['depth' => 15]));
    }

    public function testTitleFallback() {
        $view = new TemplateView();
        $view->setVariable('pageTitle', 'Page Title');

        $html = Registry::factory('Titon\View\Helper\Xhtml\XhtmlHelper');
        $html->setView($view);

        $this->assertEquals('Page Title', $this->object->title());
    }

}