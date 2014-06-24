<?php
namespace Titon\Http\Bag;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Bag\FileBag $object
 */
class FileBagTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new FileBag();
    }

    public function testNormalize() {
        $this->assertEquals([
            'file' => [
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            ]
        ], $this->object->normalize([
            'file' => [
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            ]
        ]));

        // 2 levels
        $this->assertEquals([
            'two' => [
                'file' => [
                    'name' => 'file2.png',
                    'type' => 'image/png',
                    'tmp_name' => '/tmp/phpo3HxIF',
                    'error' => 0,
                    'size' => 10554
                ]
            ]
        ], $this->object->normalize([
            'two' => [
                'name' => ['file' => 'file2.png'],
                'type' => ['file' => 'image/png'],
                'tmp_name' => ['file' => '/tmp/phpo3HxIF'],
                'error' => ['file' => 0],
                'size' => ['file' => 10554]
            ]
        ]));

        // 3 levels
        $this->assertEquals([
            'three' => [
                'two' => [
                    'file' => [
                        'name' => 'file3.png',
                        'type' => 'image/png',
                        'tmp_name' => '/tmp/phpgUtcPf',
                        'error' => 0,
                        'size' => 19571
                    ]
                ]
            ]
        ], $this->object->normalize([
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
        $this->assertEquals([
            'four' => [
                'three' => [
                    'two' => [
                        'file' => [
                            'name' => 'file4.jpg',
                            'type' => 'image/jpeg',
                            'tmp_name' => '/tmp/phpMTxSVP',
                            'error' => 0,
                            'size' => 307808
                        ]
                    ]
                ]
            ]
        ], $this->object->normalize([
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