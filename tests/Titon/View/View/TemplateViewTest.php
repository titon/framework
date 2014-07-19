<?hh
namespace Titon\View\View;

use Titon\Cache\Storage\FileSystemStorage;
use Titon\View\View;
use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\View\View\TemplateView $object
 * @property \VirtualFileSystem\FileSystem $vfs
 */
class TemplateViewTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createStructure([
            '/views/' => [
                'fallback/' => [
                    'private/' => [
                        'layouts/' => [
                            'fallback.tpl' => '<fallbackLayout><?php echo $this->getContent(); ?></fallbackLayout>'
                        ],
                        'wrappers/' => [
                            'fallback.tpl' => '<fallbackWrapper><?php echo $this->getContent(); ?></fallbackWrapper>'
                        ]
                    ]
                ],
                'private/' => [
                    'layouts/' => [
                        'default.tpl' => '<layout><?php echo $this->getContent(); ?></layout>'
                    ],
                    'partials/' => [
                        'nested/' => [
                            'include.tpl' => 'nested/include.tpl'
                        ],
                        'variables.tpl' => '<?php echo $name; ?> - <?php echo $type; ?> - <?php echo $filename; ?>'
                    ],
                    'wrappers/' => [
                        'wrapper.tpl' => '<wrapper><?php echo $this->getContent(); ?></wrapper>'
                    ],
                    'root.tpl' => 'private/root.tpl'
                ],
                'public/' => [
                    'index/' => [
                        'add.tpl' => 'add.tpl',
                        'edit.tpl' => 'edit.tpl',
                        'index.tpl' => 'index.tpl',
                        'test-include.tpl' => 'test-include.tpl <?php echo $this->open(\'nested/include\'); ?>',
                        'view.tpl' => 'view.tpl',
                        'view.xml.tpl' => 'view.xml.tpl'
                    ],
                    'root.tpl' => 'public/root.tpl'
                ]
            ],
            '/cache/' => []
        ]);

        $this->object = new TemplateView([
            $this->vfs->path('/views/'),
            $this->vfs->path('/views/fallback/')
        ]);
    }

    public function testRender() {
        $this->assertEquals('<layout>edit.tpl</layout>', $this->object->render(['index', 'edit']));

        $this->object->getEngine()->useLayout('fallback');
        $this->assertEquals('<fallbackLayout>add.tpl</fallbackLayout>', $this->object->render(['index', 'add']));

        $this->object->getEngine()->wrapWith('wrapper');
        $this->assertEquals('<fallbackLayout><wrapper>index.tpl</wrapper></fallbackLayout>', $this->object->render(['index', 'index']));

        $this->object->getEngine()->wrapWith('wrapper', 'fallback');
        $this->assertEquals('<fallbackLayout><fallbackWrapper><wrapper>view.tpl</wrapper></fallbackWrapper></fallbackLayout>', $this->object->render(['index', 'view']));

        $this->object->getEngine()->wrapWith()->useLayout('');
        $this->assertEquals('view.xml.tpl', $this->object->render(['index', 'view', 'ext' => 'xml']));
    }

    public function testRenderPrivate() {
        $this->assertEquals('<layout>public/root.tpl</layout>', $this->object->render('root'));
        $this->assertEquals('<layout>private/root.tpl</layout>', $this->object->render('root', true));
    }

    public function testRenderTemplate() {
        $this->assertEquals('add.tpl', $this->object->renderTemplate($this->object->locateTemplate(['index', 'add'])));
        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($this->object->locateTemplate(['index', 'test-include'])));

        // variables
        $this->assertEquals('Titon - partial - variables.tpl', $this->object->renderTemplate($this->object->locateTemplate('variables', View::PARTIAL), Map {
            'name' => 'Titon',
            'type' => 'partial',
            'filename' => 'variables.tpl'
        }));
    }

    public function testViewCaching() {
        $this->markTestSkipped('Test skipped; Please install titon/cache via Composer');

        $storage = new FileSystemStorage(Map {'directory' => $this->vfs->path('/cache/')});
        $this->object->setStorage($storage);

        $path = $this->object->locateTemplate(['index', 'test-include']);
        $key = md5($path);
        $cachePath = $this->vfs->path('/cache/' . $key . '.cache');

        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($path));
        $this->assertFileNotExists($cachePath);

        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($path, Map {'cache' => '+5 minutes'}));
        $this->assertFileExists($cachePath);
        $mtime = filemtime($cachePath);

        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($path, Map {'cache' => '+5 minutes'}));
        $this->assertEquals($mtime, filemtime($cachePath));

        $storage->flush();
    }

}