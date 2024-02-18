type t('e, 't) =
  | NotAsked
  | Loading
  | Fail('e)
  | Success('t);

let map: ('t => 'u, t('e, 't)) => t('e, 'u) =
  (f, remoteData) => {
    switch (remoteData) {
    | Success(v) => Success(f(v))
    | NotAsked => NotAsked
    | Loading => Loading
    | Fail(e) => Fail(e)
    };
  };

let toOption: t('e, 't) => option('t) =
  t => {
    switch (t) {
    | Success(v) => Some(v)
    | NotAsked => None
    | Loading => None
    | Fail(_) => None
    };
  };
