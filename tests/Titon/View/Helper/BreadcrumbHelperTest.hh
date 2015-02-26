<?hh
namespace Titon\View\Helper;

use Titon\Test\TestCase;
use Titon\Type\ArrayList;
use Titon\View\EngineView;

/**
 * @property \Titon\View\Helper\BreadcrumbHelper $object
 */
class BreadcrumbHelperTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $view = new EngineView(TEMP_DIR);
        $view->addHelper('html', new HtmlHelper());

        $this->object = new BreadcrumbHelper();
        $this->object->setView($view);
    }

    public function testOneCrumb(): void {
        $this->object->add('Title', '/');

        $this->assertEquals(Vector {
            '<a href="/">Title</a>' . PHP_EOL
        }, $this->object->generate());
    }

    public function testMultipleCrumbs(): void {
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

    public function testFirstLast(): void {
        $this->assertEquals(null, $this->object->first());
        $this->assertEquals(null, $this->object->last());

        $this->object
            ->add('Title', '/')
            ->add('Title 2', '/static/url', Map {'class' => 'tier2'})
            ->add('Title 3', '/view/123', Map {'class' => 'tier3'});

        $this->assertEquals(shape(
            'title' => 'Title',
            'url' => '/',
            'attributes' => Map {}
        ), $this->object->first());

        $this->assertEquals(shape(
            'title' => 'Title 3',
            'url' => '/view/123',
            'attributes' => Map {'class' => 'tier3'}
        ), $this->object->last());
    }

    public function testAppendPrepend(): void {
        $this->object->add('Base', '/');

        $this->assertEquals(new ArrayList(Vector {
            shape(
                'title' => 'Base',
                'url' => '/',
                'attributes' => Map {}
            )
        }), $this->object->getBreadcrumbs());

        $this->object->prepend('Before', '/');
        $this->object->append('After', '/');

        $this->assertEquals(new ArrayList(Vector {
            shape(
                'title' => 'Before',
                'url' => '/',
                'attributes' => Map {}
            ),
            shape(
                'title' => 'Base',
                'url' => '/',
                'attributes' => Map {}
            ),
            shape(
                'title' => 'After',
                'url' => '/',
                'attributes' => Map {}
            )
        }), $this->object->getBreadcrumbs());
    }

    public function testTitle(): void {
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

    public function testTitleFallback(): void {
        $this->object->getView()->setVariable('pageTitle', 'Page Title');

        $this->assertEquals('Page Title', $this->object->title());
    }

}
