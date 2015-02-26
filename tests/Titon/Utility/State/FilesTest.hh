<?hh // strict
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class FilesTest extends TestCase {

    public function testClass(): void {
        $_FILES = [
            'file' => [
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            ]
        ];

        Files::initialize($_FILES);

        $this->assertEquals(Map {
            'file' => Map {
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            }
        }, Files::all());

        $this->assertEquals('file1.jpg', Files::get('file.name'));
        $this->assertEquals(Map {
            'name' => 'file1.jpg',
            'type' => 'image/jpeg',
            'tmp_name' => '/tmp/phpUkYTB5',
            'error' => 0,
            'size' => 307808
        }, Files::get('file'));

        $this->assertTrue(Files::has('file'));
        $this->assertTrue(Files::has('file.error'));

        $this->assertEquals(null, Files::get('data'));
        $this->assertFalse(Files::has('file.mime_type'));
    }

    public function testPackage(): void {
        $this->assertEquals(Map {
            'file' => Map {
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            }
        }, Files::package([
            'file' => [
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            ]
        ]));

        // 2 levels
        $this->assertEquals(Map {
            'two' => Map {
                'file' => Map {
                    'name' => 'file2.png',
                    'type' => 'image/png',
                    'tmp_name' => '/tmp/phpo3HxIF',
                    'error' => 0,
                    'size' => 10554
                }
            }
        }, Files::package([
            'two' => [
                'name' => ['file' => 'file2.png'],
                'type' => ['file' => 'image/png'],
                'tmp_name' => ['file' => '/tmp/phpo3HxIF'],
                'error' => ['file' => 0],
                'size' => ['file' => 10554]
            ]
        ]));

        // 3 levels
        $this->assertEquals(Map {
            'three' => Map {
                'two' => Map {
                    'file' => Map {
                        'name' => 'file3.png',
                        'type' => 'image/png',
                        'tmp_name' => '/tmp/phpgUtcPf',
                        'error' => 0,
                        'size' => 19571
                    }
                }
            }
        }, Files::package([
            'three' => [
                'name' => [
                    'two' => ['file' => 'file3.png'],
                ],
                'type' => [
                    'two' => ['file' => 'image/png'],
                ],
                'tmp_name' => [
                    'two' => ['file' => '/tmp/phpgUtcPf'],
                ],
                'error' => [
                    'two' => ['file' => 0],
                ],
                'size' => [
                    'two' => ['file' => 19571],
                ]
            ],
        ]));

        // 4 levels
        $this->assertEquals(Map {
            'four' => Map {
                'three' => Map {
                    'two' => Map {
                        'file' => Map {
                            'name' => 'file4.jpg',
                            'type' => 'image/jpeg',
                            'tmp_name' => '/tmp/phpMTxSVP',
                            'error' => 0,
                            'size' => 307808
                        }
                    }
                }
            }
        }, Files::package([
            'four' => [
                'name' => [
                    'three' => [
                        'two' => ['file' => 'file4.jpg'],
                    ],
                ],
                'type' => [
                    'three' => [
                        'two' => ['file' => 'image/jpeg'],
                    ],
                ],
                'tmp_name' => [
                    'three' => [
                        'two' => ['file' => '/tmp/phpMTxSVP'],
                    ],
                ],
                'error' => [
                    'three' => [
                        'two' => ['file' => 0],
                    ],
                ],
                'size' => [
                    'three' => [
                        'two' => ['file' => 307808],
                    ],
                ],
            ],
        ]));
    }

}
