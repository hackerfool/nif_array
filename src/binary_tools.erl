-module(binary_tools).

-compile(export_all).

-on_load(init/0).

-define(nif_stub, nif_stub_error(?LINE)).
nif_stub_error(Line) ->
    erlang:nif_error({nif_not_loaded,module,?MODULE,line,Line}).

-ifdef(TEST).
-include_lib("eunit/include/eunit.hrl").
-endif.

init() ->
    PrivDir = case code:priv_dir(?MODULE) of
                  {error, bad_name} ->
                      EbinDir = filename:dirname(code:which(?MODULE)),
                      AppPath = filename:dirname(EbinDir),
                      filename:join(AppPath, "priv");
                  Path ->
                      Path
              end,
    erlang:load_nif(filename:join(PrivDir, ?MODULE), 0).

get_bin_address(_Ref) ->
    ?nif_stub.

new_array(_Size) ->
    ?nif_stub.

size_array(_Bin) ->
    ?nif_stub.

put_array(_Pos, _Value, _Bin) ->
  ?nif_stub.

get_array(_Pos, _Bin) ->
  ?nif_stub.

test() ->
  List = lists:seq(1, 10000),
  A = new_array(10000),
  EA = array:from_list(List),
  [put_array(S - 1, S, A) || S <- List], %array begin from 0,list begin from 1
  G = [random:uniform(9999) || _ <- List],



  {LT, LR} = timer:tc(fun()-> [lists:nth(Pos, List) || Pos <- G] end),
  {AT, AR} = timer:tc(fun()-> [get_array(Pos - 1, A) || Pos <- G] end),
  {EAT, EAR} = timer:tc(fun()-> [array:get(Pos - 1, EA) || Pos <- G] end),
  LR = AR,
  LR = EAR,
  io:format("List random nth : ~p~n", [LT]),
  io:format("Array random get : ~p~n", [AT]),
  io:format("Erlang Array random get : ~p~n", [EAT]).
%% ===================================================================
%% EUnit tests
%% ===================================================================
