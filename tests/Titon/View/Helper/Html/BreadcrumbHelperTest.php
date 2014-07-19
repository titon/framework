<?hh
namespace Titon\View\Helper\Html;

use Titon\Utility\Registry;
use Titon\Test\TestCase;
use Titon\View\View\TemplateView;

/**
 * @property \Titon\View\Helper\Html\BreadcrumbHelper $object
 */
class BreadcrumbHelperTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new BreadcrumbHelper();
    }

    public function testOneCrumb() {
        $this->object->add('Title', '/');

        $this->assertEquals(Vector {
            '<a href="/">Title</a>' . PHP_EOL
        }, $this->object->generate());
    }

    public function testMultipleCrumbs() {
        $this->object
            ->add('Title', '/')
            ->add('Title 2', '/static/url', Map {'class' => 'tier2'})
            ->add('Title 3', '/main/index/view/123', Map {'class' => 'tier3'});

        $this->assertEquals(Vector {
            '<a href="/">Title</a>' . PHP_EOL,
            '<a class="tier2" href="/static/url">Title 2</a>' . PHP_EOL,
            '<a class="tier3" href="/main/index/view/123">Title 3</a>' . PHP_EOL
        }, $this->object->generate());
    }

    public function testFirstList() {
        $this->assertEquals(null, $this->object->first());
        $this->assertEquals(null, $this->object->last());

        $this->object
            ->add('Title', '/')
            ->add('Title 2', '/static/url', Map {'class' => 'tier2'})
            ->add('Title 3', ['action' => 'view', 123], Map {'class' => 'tier3'});

        $this->assertEquals(Map {
            'title' => 'Title',
            'url' => '/',
            'attributes' => Map {}
        }, $this->object->first());

        $this->assertEquals(Map {
            'title' => 'Title 3',
            'url' => ['action' => 'view', 123],
            'attributes' => Map {'class' => 'tier3'}
        }, $this->object->last());
    }

    public function testAppendPrepend() {
        $this->object->add('Base', '/');

        $this->assertEquals(Vector {
            Map {
                'title' => 'Base',
                'url' => '/',
                'attributes' => Map {}
            }
        }, $this->object->all());

        $this->object->prepend('Before', '/');
        $this->object->append('After', '/');

        $this->assertEquals(Vector {
            Map {
                'title' => 'Before',
                'url' => '/',
                'attributes' => Map {}
            }, Map {
                'title' => 'Base',
                'url' => '/',
                'attributes' => Map {}
            }, Map {
                'title' => 'After',
                'url' => '/',
                'attributes' => Map {}
            }
        }, $this->object->all());
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
        $this->assertEquals('Base - A - C - D - E - F - G', $this->object->title('Base', Map {'depth' => 5}));
        $this->assertEquals('Base - A - G', $this->object->title('Base', Map {'depth' => 1}));
        $this->assertEquals('Base - G - F - E - A', $this->object->title('Base', Map {'reverse' => true}));
        $this->assertEquals('G > F > E > A', $this->object->title('', Map {'reverse' => true, 'separator' => ' > '}));
        $this->assertEquals('Base - A - B - C - D - E - F - G', $this->object->title('Base', Map {'depth' => 15}));
    }

    public function testTitleFallback() {
        $view = new TemplateView(Vector {'/'});
        $view->setVariable('pageTitle', 'Page Title');

        $html = Registry::factory('Titon\View\Helper\Html\HtmlHelper');
        $html->setView($view);

        $this->assertEquals('Page Title', $this->object->title());
    }

}