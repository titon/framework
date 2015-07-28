<?hh

return Map {
    'ssn' => '###-##-####',

    // Phone
    'phone' => Map {
        7 => '###-####',
        10 => '(###) ###-####',
        11 => '# (###) ###-####'
    },

    // Datetime
    'date' => '%m/%d/%Y',
    'time' => '%I:%M%p',
    'datetime' => '%m/%d/%Y %I:%M%p',

    // Numbers
    'number' => Map {
        'thousands' => ',',
        'decimals' => '.',
        'places' => '2'
    },

    // Currency
    'currency' => Map {
        'code' => 'USD #',
        'dollar' => '$#',
        'cents' => '#&cent;',
        'negative' => '(#)',
        'use' => 'dollar'
    }
};
