<?hh
namespace Titon\View\Helper;

use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\View\Helper\AssetHelper $object
 */
class AssetHelperTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createDirectory('/css/');
        $this->vfs->createFile('/css/test.css');

        $this->object = new AssetHelper($this->vfs->path('/'));
    }

    public function testScripts() {
        $this->object
            ->addScript('script.js')
            ->addScript('path/commons.js', 'header')
            ->addScript('path/no-extension')
            ->addScript('/a/really/really/deep/path/include.js', 'header');

        $this->assertEquals('', $this->object->scripts('fakeLocation'));

        $this->assertEquals(
            '<script src="path/commons.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="/a/really/really/deep/path/include.js" type="text/javascript"></script>' . PHP_EOL
        , $this->object->scripts('header'));

        $this->assertEquals(
            '<script src="script.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="path/no-extension.js" type="text/javascript"></script>' . PHP_EOL
        , $this->object->scripts('footer'));
    }

    public function testScriptsOrdering() {
        $this->object
            ->addScript('script.js', 'footer', 3)
            ->addScript('path/commons.js', 'footer', 2)
            ->addScript('path/no-extension', 'footer')
            ->addScript('/a/really/really/deep/path/include.js', 'footer', 5);

        $this->assertEquals(
            '<script src="path/commons.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="script.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="path/no-extension.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="/a/really/really/deep/path/include.js" type="text/javascript"></script>' . PHP_EOL
        , $this->object->scripts('footer'));
    }

    public function testScriptsEnv() {
        $this->object
            ->addScript('script.js', 'footer', 30, 'dev')
            ->addScript('path/commons.js', 'footer', 30, 'prod')
            ->addScript('path/no-extension', 'footer', 30, 'staging')
            ->addScript('/a/really/really/deep/path/include.js', 'footer', 30, 'dev');

        $this->assertEquals(
            '<script src="script.js" type="text/javascript"></script>' . PHP_EOL .
            '<script src="/a/really/really/deep/path/include.js" type="text/javascript"></script>' . PHP_EOL
        , $this->object->scripts('footer', 'dev'));
    }

    public function testStylesheets() {
        $this->object
            ->addStylesheet('style.css')
            ->addStylesheet('a/really/deep/path/with/no/extension/style.css')
            ->addStylesheet('mobile.css', Map {'media' => 'mobile'});

        $this->assertEquals(
            '<link href="style.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="a/really/deep/path/with/no/extension/style.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="mobile.css" media="mobile" rel="stylesheet" type="text/css">' . PHP_EOL
        , $this->object->stylesheets());
    }

    public function testStylesheetsOrdering() {
        $this->object
            ->addStylesheet('style.css', Map {'media' => 'handheld'}, 3)
            ->addStylesheet('a/really/deep/path/with/no/extension/style.css', Map {'media' => 'screen'}, 1)
            ->addStylesheet('mobile.css', Map {'media' => 'mobile'}, 2);

        $this->assertEquals(
            '<link href="a/really/deep/path/with/no/extension/style.css" media="screen" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="mobile.css" media="mobile" rel="stylesheet" type="text/css">' . PHP_EOL .
            '<link href="style.css" media="handheld" rel="stylesheet" type="text/css">' . PHP_EOL
        , $this->object->stylesheets());
    }

    public function testStylesheetsEnv() {
        $this->object
            ->addStylesheet('style.css', Map {'media' => 'handheld'}, 30, 'dev')
            ->addStylesheet('a/really/deep/path/with/no/extension/style.css', Map {'media' => 'screen'}, 30, 'staging')
            ->addStylesheet('mobile.css', Map {'media' => 'mobile'}, 30, 'prod');

        $this->assertEquals('<link href="style.css" media="handheld" rel="stylesheet" type="text/css">' . PHP_EOL, $this->object->stylesheets('dev'));
    }

    public function testPreparePath() {
        $helper = $this->object;

        $this->assertEquals('style.css', $helper->preparePath('style', 'css'));
        $this->assertEquals('style.css', $helper->preparePath('style.css', 'css'));
        $this->assertEquals('/css/style.css', $helper->preparePath('/css/style', 'css'));
        $this->assertEquals('/css/style.min.css', $helper->preparePath('/css/style.min', 'css'));
        $this->assertEquals('/css/style.min.css?v=1', $helper->preparePath('/css/style.min?v=1', 'css'));

        // Should not change
        $this->assertEquals('//domain.com/style', $helper->preparePath('//domain.com/style', 'css'));
        $this->assertEquals('http://domain.com/script', $helper->preparePath('http://domain.com/script', 'js'));
        $this->assertEquals('https://domain.com/asset?version=123', $helper->preparePath('https://domain.com/asset?version=123', 'js'));
    }

    public function testTimestamping() {
        $this->object->addStylesheet('/css/test.css');

        $this->assertRegExp('/<link href="\/css\/test\.css\?([0-9]+)" media="screen" rel="stylesheet" type="text\/css">/', $this->object->stylesheets());
    }

}